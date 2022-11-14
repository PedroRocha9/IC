# IC - Project 2

## Compile

Go to `proj2/opencv-src` and run `make all`

## Testing

Go to `proj2/opencv-bin`

<br>

### Exercise 1

```bash
./cp_image ../iamge_files/lena.ppm lena_copy.ppm
```  
To check the results either view the image by running the command with the word "view" or open the output file

<br>

### Exercise 2

**Negative image**
```bash
./negative_image ../iamge_files/monarch.ppm monarch_negative.ppm
```

**Mirror image in horizontal axis**
```bash
./mirror_image ../iamge_files/airplane.ppm airplane_h.ppm h
```

**Mirror image in vertical axis**
```bash
./mirror_image ../iamge_files/airplane.ppm airplane_v.ppm v
```

**Rotated image 90º**
```bash
./rotate_image ../iamge_files/arial.ppm arial_rot90.ppm 90
```

**Rotated image 180º**
```bash
./rotate_image ../iamge_files/arial.ppm arial_rot90.ppm 90
```

**Increase Light image**
```bash
./bright_image ../iamge_files/bike3.ppm bike3_h50.ppm 50
```

**Decrease Light image**
```bash
./bright_image ../iamge_files/bike3.ppm bike3_l50.ppm -50
```

To check the results either view the image by running the command with the word "view" or open the output file

<br>
