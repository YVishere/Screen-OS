##############################################
## @Author:  Aditya Yellapuntula Venkata
## In-house converter for images in specified format to C arrays
## Generates a C-header file that is ready to be inluded right away
##############################################

import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
import sys


## Convert video to RGB332 format with all frames stacked vertically
# Process each frame of an animated video into RGB332 format
# Generate a C header file with frames stacked in a single array
# Includes helper macros for accessing individual frames
#
# @param video_path Path to the video file
# @param output_name Name for the output header file (without extension)
# @param max_frames Maximum number of frames to process (None for all)
# @param rotate_k Number of 90 degree rotations to apply to each frame (0, 1, 2, or 3)
def convert_video_to_rgb332_frames(video_path, output_name, max_frames=None, rotate_k = 0):
    """
    Convert a video file to a C header file with each frame as a separate array.
    Frames are stacked vertically in the data structure.
    
    Args:
        video_path (str): Path to the video file
        output_name (str): Name for the output header file (without extension)
    """
    import numpy as np
    from wand.image import Image
    from PIL import Image as PILImage
    import io
    
    # Open the video and coalesce frames
    with Image(filename=video_path) as img:
        img.coalesce()
        
        # Get basic information
        num_frames = len(img.sequence)
        if max_frames is not None and max_frames > 0:
            num_frames = min(num_frames, max_frames)
        width = img.width
        height = img.height
        print(f"Processing video with {num_frames} frames, dimensions: {width}x{height}")
        
        # Array to hold all frame data
        all_frames_data = []
        
        # Convert each frame
        for i, frame in enumerate(img.sequence):
            if max_frames is not None and i >= max_frames:
                break

            # Convert Wand image to PIL image
            frame_img = frame.clone()
            frame_blob = frame_img.make_blob(format='bmp')
            pil_img = PILImage.open(io.BytesIO(frame_blob))
            
            # Convert to RGB if not already
            if pil_img.mode != 'RGB':
                pil_img = pil_img.convert('RGB')
            
            # Convert to numpy array
            img_array = np.array(pil_img)
            img_array = np.rot90(img_array, k=rotate_k)
            
            # Convert to RGB332 (8-bit, RRRGGGBB)
            r = (img_array[:,:,0] >> 5) & 0x07  # Extract top 3 bits for R
            g = (img_array[:,:,1] >> 5) & 0x07  # Extract top 3 bits for G
            b = (img_array[:,:,2] >> 6) & 0x03  # Extract top 2 bits for B
            
            # Pack into a single byte per pixel: RRRGGGBB
            rgb332 = (r << 5) | (g << 2) | b
            
            # Flatten the array and add to collection
            all_frames_data.append(rgb332.flatten())
            print(f"Processed frame {i+1}/{num_frames}")
        
        # Stack all frames into a single array
        stacked_data = np.concatenate(all_frames_data)
        
        # Create header file
        header_file = f"{output_name}.h"
        with open(header_file, "w") as f:
            f.write(f"// Auto-generated RGB332 data for {video_path}\n")
            f.write(f"// Contains {num_frames} frames of {width}x{height} pixels\n\n")
            
            f.write(f"#ifndef _{output_name.upper()}_H_\n")
            f.write(f"#define _{output_name.upper()}_H_\n\n")

            f.write("#include <Arduino.h>\n\n")
            
            f.write(f"#define {output_name.upper()}_WIDTH {width}\n")
            f.write(f"#define {output_name.upper()}_HEIGHT {height}\n")
            f.write(f"#define {output_name.upper()}_FRAMES {num_frames}\n")
            f.write(f"#define {output_name.upper()}_FRAME_SIZE ({width}*{height})\n\n")
            
            f.write(f"const uint8_t {output_name}[] = {{\n")
            
            # Write data in rows of 12 values
            for i in range(0, len(stacked_data), 12):
                row = stacked_data[i:i+12]
                f.write("    " + ", ".join([f"0x{val:02X}" for val in row]))
                if i + 12 < len(stacked_data):
                    f.write(",")
                f.write("\n")
            
            f.write("};\n\n")
            
            # Add helper macros for accessing individual frames
            f.write("// Helper macro to access a specific frame\n")
            f.write(f"#define {output_name.upper()}_FRAME(n) ")
            f.write(f"(&{output_name}[(n) * {output_name.upper()}_FRAME_SIZE])\n\n")
            
            f.write("#endif\n")
                
        # Save raw array data to a text file
        txt_filename = f"{output_name}_array.txt"
        with open(txt_filename, "w") as txt_file:
            # Write the array values
            for val in stacked_data:
                txt_file.write(f"{val}\n")

        print(f"Saved raw array data to {txt_filename}")
        
        print(f"Successfully created header file: {header_file}")
        print(f"Total size: {len(stacked_data)} bytes")

## Convert video to RGB332 format and save each frame as a binary file
# Process each frame of an animated video into RGB332 format
# Save each frame as a binary file in the specified output folder
#
# @param video_path Path to the video file
# @param output_folder Path to the output folder where binary files will be stored
# @param max_frames Maximum number of frames to process (None for all)
# @param rotate_k Number of 90 degree rotations to apply to each frame (0, 1, 2, or 3)
def convert_video_to_rgb332_bin_frames(video_path, output_folder, max_frames=None, rotate_k=0):
    """
    Convert a video file to a series of binary files, each containing a frame in RGB332 format.
    
    Args:
        video_path (str): Path to the video file
        output_folder (str): Path to the output folder where binary files will be stored
        max_frames (int, optional): Maximum number of frames to process. Defaults to None (all frames).
        rotate_k (int, optional): Number of 90 degree rotations to apply. Defaults to 0.
    """
    import numpy as np
    from wand.image import Image
    from PIL import Image as PILImage
    import io
    import os
    import shutil
    
    # Create output directory, clearing it if it exists
    if os.path.exists(output_folder):
        shutil.rmtree(output_folder)
    os.makedirs(output_folder)
    
    # Open the video and coalesce frames
    with Image(filename=video_path) as img:
        img.coalesce()
        
        # Get basic information
        num_frames = len(img.sequence)
        if max_frames is not None and max_frames > 0:
            num_frames = min(num_frames, max_frames)
        width = img.width
        height = img.height
        print(f"Processing video with {num_frames} frames, dimensions: {width}x{height}")
        
        # Convert each frame
        for i, frame in enumerate(img.sequence):
            if max_frames is not None and i >= max_frames:
                break

            # Convert Wand image to PIL image
            frame_img = frame.clone()
            frame_blob = frame_img.make_blob(format='bmp')
            pil_img = PILImage.open(io.BytesIO(frame_blob))
            
            # Convert to RGB if not already
            if pil_img.mode != 'RGB':
                pil_img = pil_img.convert('RGB')
            
            # Convert to numpy array
            img_array = np.array(pil_img)
            img_array = np.rot90(img_array, k=rotate_k)
            
            # Get frame dimensions (might be different after rotation)
            frame_height, frame_width, _ = img_array.shape
            
            # Convert to RGB332 (8-bit, RRRGGGBB)
            r = (img_array[:,:,0] >> 5) & 0x07  # Extract top 3 bits for R
            g = (img_array[:,:,1] >> 5) & 0x07  # Extract top 3 bits for G
            b = (img_array[:,:,2] >> 6) & 0x03  # Extract top 2 bits for B
            
            # Pack into a single byte per pixel: RRRGGGBB
            rgb332 = (r << 5) | (g << 2) | b
            
            # Save to binary file
            bin_filename = os.path.join(output_folder, f"frame{i+1}.bin")
            with open(bin_filename, "wb") as bin_file:
                # Convert to bytes and write
                rgb332.astype(np.uint8).tofile(bin_file)
            
            print(f"Saved frame {i+1}/{num_frames} to {bin_filename} - Dimensions: {frame_width}x{frame_height}")
        
        # Create info file with metadata
        info_filename = os.path.join(output_folder, "info.txt")
        with open(info_filename, "w") as info_file:
            info_file.write(f"Video: {os.path.basename(video_path)}\n")
            info_file.write(f"Frames: {num_frames}\n")
            info_file.write(f"Width: {width}\n")
            info_file.write(f"Height: {height}\n")
            info_file.write("Format: RGB332 (RRRGGGBB)\n")
        
        print(f"Successfully exported {num_frames} frames to {output_folder}")
        print(f"Frame size: {width*height} bytes")

## Convert BMP to RGB332 format and verify the output
# Generate a text file for image data
# Generate a C header file for the image data ready to be included in an Arduino sketch
#
# @param image_path Path to the image file
# @param image_name Name of the image file
# @param rotate_k Number of 90 degree rotations to apply to the image
def convert_bmp_to_rgb332(image_path, image_name, rotate_k):
    """Convert BMP to RGB332 format and verify the conversion."""
    # Load the image
    img = Image.open(image_path)
    print(f"Original image size: {img.size}, mode: {img.mode}")
    
    # Convert to RGB if not already
    if img.mode != 'RGB':
        img = img.convert('RGB')
    
    # Get image as numpy array
    img_array = np.array(img)
    img_array = np.rot90(img_array, k=rotate_k)
    height, width, _ = img_array.shape
    
    # Convert to RGB332 (1 byte per pixel)
    # 3 bits for R (0-7), 3 bits for G (0-7), 2 bits for B (0-3)
    r = (img_array[:,:,0] >> 5) & 0x07  # Extract top 3 bits
    g = (img_array[:,:,1] >> 5) & 0x07  # Extract top 3 bits
    b = (img_array[:,:,2] >> 6) & 0x03  # Extract top 2 bits
    
    # Pack into a single byte per pixel: RRRGGGBB
    rgb332 = (r << 5) | (g << 2) | b
    
    # Flatten the array for C export
    flat_rgb332 = rgb332.flatten()

    #Save array in a text file
    txt_filename = f"{image_name}_array.txt"
    with open(txt_filename, "w") as txt_file:
        txt_file.write(f"# RGB332 image data ({width}x{height})\n")
        txt_file.write(f"# Width: {width}\n")
        txt_file.write(f"# Height: {height}\n")
        txt_file.write(f"# Format: One byte per pixel, RRRGGGBB\n")
        txt_file.write("#\n")
        txt_file.write("# Array values (decimal):\n")
        rgb_mat = np.reshape(rgb332, (height, width))
        for row in rgb_mat:
            txt_file.write(" ".join([f"{val:3d}" for val in row]) + "\n")
    
    print(f"Saved array data to {txt_filename}")
    
    # Create header file
    filename = image_name+".h"
    with open(filename, "w") as f:
        f.write("#include <Arduino.h>\n")   
        f.write(f"// RGB332 image data ({width}x{height})\n")
        f.write(f"#define {image_name}_WIDTH {width}\n")
        f.write(f"#define {image_name}_HEIGHT {height}\n")
        f.write("const uint8_t "+image_name+"[] = {\n")
        
        # Write data in rows of 16 values
        for i in range(0, len(flat_rgb332), 16):
            row = flat_rgb332[i:i+16]
            f.write("    " + ", ".join([f"0x{val:02X}" for val in row]))
            if i + 16 < len(flat_rgb332):
                f.write(",")
            f.write("\n")
        f.write("};\n")
    
    print(f"Saved C array to image_rgb332.h")
    
    # Reconstruct image for validation
    # Convert RGB332 back to RGB888
    # Better scaling that preserves brightness
    r_recon = np.uint8(((rgb332 >> 5) & 0x07) * (255.0 / 7.0))
    g_recon = np.uint8(((rgb332 >> 2) & 0x07) * (255.0 / 7.0))
    b_recon = np.uint8((rgb332 & 0x03) * (255.0 / 3.0))
    
    recon_array = np.zeros((height, width, 3), dtype=np.uint8)
    recon_array[:,:,0] = r_recon
    recon_array[:,:,1] = g_recon
    recon_array[:,:,2] = b_recon
    
    # Convert back to PIL Image
    recon_img = Image.fromarray(recon_array)
    
    # Display both images side by side
    plt.figure(figsize=(12, 6))
    plt.subplot(1, 2, 1)
    plt.title("Original Image")
    plt.imshow(img)
    plt.axis('off')
    
    plt.subplot(1, 2, 2)
    plt.title("Reconstructed Image (RGB332)")
    plt.imshow(recon_img)
    plt.axis('off')
    
    plt.tight_layout()
    plt.show()
    
    return flat_rgb332, (width, height)

if __name__ == "__main__":
    # Get image path from command line or use default
    # rotate = 3              #Rotate image by k * 90 degrees
    # extension = ".jpeg"     #Image extension
    # image_name = "kar"      #Image name
    # image_path = f"{image_name}{extension}" 
    # convert_bmp_to_rgb332(image_path, image_name, rotate)

    convert_video_to_rgb332_bin_frames("disint.gif", "output_frames", max_frames=10, rotate_k=1)

