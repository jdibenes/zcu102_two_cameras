# ZCU102 Two Camera Design 

This is a standalone design for using two IMX274 (LI-IMX274MIPI-FMC) cameras with the ZCU102 Evaluation Board. The main application (helloworld.c) captures an image from both cameras when one of the 5 push buttons (SW14 to SW18) is pressed and stores the two images on the SD Card. Image format is 3840x2160 (4K), 16 bits per pixel YUV 4:2:2 (Packed YUYV), YUV are 8 bits each. The block diagram tcl script (bd_zcu102_2cam.tcl) is for Vivado 2017.4.

2022 UPDATE: Install the patch first: https://support.xilinx.com/s/article/76960?language=en_US.

## Image Format

```
          lsb                                 msb
row    0: Y0 U0 Y1 V1 Y2 U2 Y3 V3 ... Y3839 V3839
row    1: Y0 U0 Y1 V1 Y2 U2 Y3 V3 ... Y3839 V3839
...
row 2159: Y0 U0 Y1 V1 Y2 U2 Y3 V3 ... Y3839 V3839
```

## Instructions

### Vivado

1. Open Vivado and create a new project (File->New Project). Select RTL Project and set Do not specify sources at this time. Select Boards and then select Zynq UltraScale+ ZCU102 Evaluation Board.

2. After creating the project, run bd_zcu102_2cam.tcl (Tools->Run Tcl Script...).

3. Add the constraints file zcu102_ds.xdc (Add Sources->Add or create constraints->Add Files).

4. Create the HDL wrapper. Right click on the block diagram (design_1) in the sources window and select Create HDL Wrapper. Let Vivado manage wrapper and auto-update.

5. Generate Bitstream (this will take a while).

6. Export hardware, including bitstream (File->Export->Export Hardware...).

7. Launch SDK (File->Launch SDK).

### Xilinx SDK

8. Create BSP (File->New->Board Support Package) and enable xilffs.

9. Create application project (File->New->Application Project) and Use existing BSP (from step 8). Select the Hello World template.

10. Right click on the src folder in the application project. Select Import. Select General->File System. Import from the directory containing helloworld.c. Select capture.c, capture.h, helloworld.c, imx274.c, imx274.h and the s2mm_PL folder. Your application project should look like this:

```
> Binaries
> Includes
> Debug
v src
    > s2mm_PL
    > capture.c
    > capture.h
    > helloworld.c
    > imx274.c
    > imx274.h
    > platform_config.h
    > platform.c
    > platform.h
      lscript.ld
```

11. Right click on the application project and select Clean Project.

12. Connect the zcu102 board to the PC, insert the SD card on the board, and turn it on. In the SDK Terminal tab, click on the green plus button and select the usb port corresponding to the board (usually USB0).

13. Program the FPGA (Xilinx->Program FPGA).

14. Run the application. Open helloworld.c and click anywhere on the file. Select Run->Run As->Launch on Hardware (System Debugger).

15. Wait until the message "Ready to capture!" is shown in the SDK Terminal.

16. Press any of the push buttons SW14, SW15, SW16, SW17, or SW18 to capture images from both cameras. The images will be stored on the SD card.

