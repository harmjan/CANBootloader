This project is a distributed CAN bootloader for the LPC1769.

# Project structure

The entire project was build with the LPCXpresso IDE. The subprojects are:

 - Bootloader, the main bootloader that gets programmed onto every node.
 - Programmer, the special node in the network that receives code from the developers computer and translates it into our CAN protocol.
 - Bootloaderlib, the library with shared code between the Bootloader and Programmer project.
 - Host, the code that gets a binary via a FTDI chip to the special Programmer node.
 - User Application, a small user application that we used to test if the programmer works. It flashes a led on the LPCXpresso.
 - HelloWorldTest, a test project to get familiar with the development environment.
 - CMSISv2.10_LPC17xx and LPC17xx-Drivers, driver and setup code from NXP, ARM and Code RED to setup embedded projects for the LPC1769.

# Design

The design is documented in our report available on <http://repository.tudelft.nl/view/ir/uuid%3A23211b17-11ce-4cc5-8f84-35766f7a975f/>.
Important points are:
 - The bootloader is located at the top of the flash so the application you are flashing onto the nodes does not need to be linked differently from the developers test setup.
 - The design is modular so you should be able to replace CAN with another bus protocol or port the application to another ARM processor without to many problems.
