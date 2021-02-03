# ZCU102 Two Camera Design 

This is a standalone design for using two IMX274 (LI-IMX274MIPI-FMC) cameras with the ZCU102 Evaluation Board. The main application (helloworld.c) captures an image from both cameras when one of the 5 push buttons (SW14 to SW18) is pressed and stores the two images on the SD Card. Image format is 3840x2160 (4K), 16 bits per pixel YUV 4:2:2 (Packed YUYV), YUV are 8 bits each. The block diagram tcl script (bd_zcu102_2cam.tcl) is for Vivado 2017.4.

## Image Format

```
          lsb                                 msb
row    0: Y0 U0 Y1 V1 Y2 U2 Y3 V3 ... Y3839 V3839
row    1: Y0 U0 Y1 V1 Y2 U2 Y3 V3 ... Y3839 V3839
...
row 2159: Y0 U0 Y1 V1 Y2 U2 Y3 V3 ... Y3839 V3839
```

## Instructions

1. Open Vivado and create a new project (File->New Project).

2. Select RTL Project and set Do not specify sources at this time.

3. Select Boards and then select Zynq UltraScale+ ZCU102 Evaluation Board.

4. After creating the project, run bd_zcu102_2cam.tcl (Tools->Run Tcl Script...).

5. Add the constraints file zcu102_ds.xdc (Add Sources->Add or create constraints->Add Files).

6. Create the HDL wrapper. Right click on the block diagram (design_1) in the sources window and select Create HDL Wrapper. Let Vivado manage wrapper and auto-update.

7. Generate Bitstream



7. Export hardware, including bitstream (File->Export->Export Hardware...)

8. Launch SDK (File->Launch SDK)

9. Create BSP and enable xilffs

10. Create application project (hello world template and use BSP from step 9)

11. Import all the files in s2mm_PL (import from filesystem), helloworld.c, imx274.c and imx274.h

