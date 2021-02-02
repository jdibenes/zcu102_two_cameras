###################
# Pin Constraints #
###################
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
                               -group [get_clocks -of [get_pins */clk_wiz_1/inst/mmcme4_adv_inst/CLKOUT3]] \
                               -group [get_clocks -of [get_pins */clk_wiz_1/inst/mmcme4_adv_inst/CLKOUT4]]
                               
# compress bitstream
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

# override default routing for DPHY clock
set_property CLOCK_DEDICATED_ROUTE BACKBONE [get_nets */clk_wiz_1/inst/clk_out5]
