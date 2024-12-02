#!/bin/sh

# Loop through all .jpg files in the specified directory
for img in ./*.jpg; do
  # Check if the file exists (in case there are no .jpg files)
  if [[ -f "$img" ]]; then
    # Get the base filename without extension
    base_name=$(basename "$img" .jpg)
    # Convert to .png
    magick convert "$img" "./$base_name.png"
    echo "Converted $img to $base_name.png"
  fi
done

echo "Conversion completed."

