###################
# Pin Constraints #
###################

#
# VGA
#

set_property PACKAGE_PIN A20 [get_ports vga_g[0]]
set_property PACKAGE_PIN B20 [get_ports vga_g[1]]
set_property PACKAGE_PIN A22 [get_ports vga_g[2]]
set_property PACKAGE_PIN A21 [get_ports vga_g[3]]

set_property PACKAGE_PIN B21 [get_ports vga_r[0]]
set_property PACKAGE_PIN C21 [get_ports vga_r[1]]
set_property PACKAGE_PIN C22 [get_ports vga_r[2]]
set_property PACKAGE_PIN D21 [get_ports vga_r[3]]

set_property PACKAGE_PIN D20 [get_ports vga_b[0]]
set_property PACKAGE_PIN E20 [get_ports vga_b[1]]
set_property PACKAGE_PIN D22 [get_ports vga_b[2]]
set_property PACKAGE_PIN E22 [get_ports vga_b[3]]

set_property PACKAGE_PIN F20 [get_ports vga_h]
set_property PACKAGE_PIN G20 [get_ports vga_v]

set_property IOSTANDARD LVCMOS33 [get_ports vga_b]
set_property IOSTANDARD LVCMOS33 [get_ports vga_g]
set_property IOSTANDARD LVCMOS33 [get_ports vga_r]
set_property IOSTANDARD LVCMOS33 [get_ports vga_h]
set_property IOSTANDARD LVCMOS33 [get_ports vga_v]

#
# MIPI CSI-2 - I2C IMX274 Sensor
#

# HPC0
# PL Port            Pin  Schematic    FMC
#
# sensor_iic_scl_io  L15  HPC0_LA26_P  D26  FMC_SCL
# sensor_iic_sda_io  K15  HPC0_LA26_N  D27  FMC_SDA
#
set_property PACKAGE_PIN L15 [get_ports sensor0_iic_scl_io]
set_property PACKAGE_PIN K15 [get_ports sensor0_iic_sda_io]
set_property PULLUP true [get_ports sensor0_iic_*]
set_property IOSTANDARD HSUL_12_DCI [get_ports sensor0_iic_*]

# HPC1
# PL Port            Pin  Schematic    FMC
#
# sensor_iic_scl_io  T12  HPC0_LA26_P  D26  FMC_SCL
# sensor_iic_sda_io  R12  HPC0_LA26_N  D27  FMC_SDA
#
set_property PACKAGE_PIN T12 [get_ports sensor1_iic_scl_io]
set_property PACKAGE_PIN R12 [get_ports sensor1_iic_sda_io]
set_property PULLUP true [get_ports sensor1_iic_*]
set_property IOSTANDARD HSUL_12_DCI [get_ports sensor1_iic_*]

#
# MIPI CSI-2 - GPIO - CAM_FLASH, CAM_XCE, CAM_RST
#

# HPC0
# PL Port               Pin   Schematic    FMC
#
# sensor_gpio_rst       M14   HPC0_LA22_N  G25  FMC_RST
# sensor_gpio_spi_cs_n  M10   HPC0_LA27_P  C26  FMC_SPI_CS_N
# sensor_gpio_flash     AA12  HPC0_LA16_N  G19  FMC_FLASH
#
set_property PACKAGE_PIN M14 [get_ports {sensor0_gpio_rst}]
set_property PACKAGE_PIN M10 [get_ports {sensor0_gpio_spi_cs_n}]
set_property PACKAGE_PIN AA12 [get_ports {sensor0_gpio_flash}]
set_property IOSTANDARD LVCMOS12 [get_ports sensor0_gpio_*]

# HPC1
# PL Port               Pin   Schematic    FMC
#
# sensor_gpio_rst       AG11  HPC0_LA22_N  G25  FMC_RST
# sensor_gpio_spi_cs_n  U10   HPC0_LA27_P  C26  FMC_SPI_CS_N
# sensor_gpio_flash     AG9   HPC0_LA16_N  G19  FMC_FLASH
#
set_property PACKAGE_PIN AG11 [get_ports {sensor1_gpio_rst}]
set_property PACKAGE_PIN U10 [get_ports {sensor1_gpio_spi_cs_n}]
set_property PACKAGE_PIN AG9 [get_ports {sensor1_gpio_flash}]
set_property IOSTANDARD LVCMOS12 [get_ports sensor1_gpio_*]

################
# Clock Groups #
################

# There is no defined phase relationship, hence they are treated as asynchronous
set_clock_groups -asynchronous -group [get_clocks -of [get_pins */clk_wiz_1/inst/mmcme4_adv_inst/CLKOUT0]] \
                               -group [get_clocks -of [get_pins */clk_wiz_1/inst/mmcme4_adv_inst/CLKOUT1]] \
                               -group [get_clocks -of [get_pins */clk_wiz_1/inst/mmcme4_adv_inst/CLKOUT2]] \
                               -group [get_clocks -of [get_pins */clk_wiz_1/inst/mmcme4_adv_inst/CLKOUT3]] \
                               -group [get_clocks -of [get_pins */clk_wiz_1/inst/mmcme4_adv_inst/CLKOUT4]] \
                               -group [get_clocks -of [get_pins */vid_phy_controller_0/inst/gt_usrclk_source_inst/tx_mmcm.txoutclk_mmcm0_i/mmcm_adv_inst/CLKOUT2]]
                               
# compress bitstream
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

# override default routing for DPHY clock
set_property CLOCK_DEDICATED_ROUTE BACKBONE [get_nets */clk_wiz_1/inst/clk_out5]
