# zcu102_2cam

zcu102 two camera setup 

Main application captures an image from both cameras when one the 5 push buttons is pressed and stores the two images in the SD Card.


Vivado 2017.4

Board: zcu102

FMC_HPC0: LI-IMX274MIPI-FMC V1.0

FMC_HPC1: LI-IMX274MIPI-FMC V1.0


1. Open Vivado and create a new project for zcu102 board

3. Run bd_zcu102_2cam.tcl (tools->run tcl script)

4. Add constraints (zcu102_ds.xdc)

5. Create HDL wrapper

6. Generate Bitstream

7. Export hardware (check include bitstream)

8. Launch SDK

9. Create BSP and enable xilffs

10. Create application project (hello world template and use BSP from step 9)

11. Import all the files in s2mm_PL (import from filesystem), helloworld.c, imx274.c and imx274.h

