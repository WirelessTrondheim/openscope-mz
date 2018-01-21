CC:=${HOME}/.arduino15/packages/Digilent/tools/xc32-tools/xc32-1.43/bin/xc32-gcc
CXX:=${HOME}/.arduino15/packages/Digilent/tools/xc32-tools/xc32-1.43/bin/xc32-g++

CFLAGS:=-g -O0 -w -mno-smart-io -ffunction-sections -fdata-sections  -mdebugger -Wcast-align -fno-short-double -ftoplevel-reorder -MMD  -mnewlib-libc -mprocessor=32MZ2048EFG124 -DF_CPU=200000000UL  -DARDUINO=10609 -D_BOARD_OPENSCOPE_ -DMPIDEVER=16777998 -DMPIDE=150 -DIDE=Arduino -I${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32 -I${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/variants/OpenScope -I.

CXXFLAGS:=-g -O0 -w -mno-smart-io -fno-exceptions -ffunction-sections -fdata-sections -mdebugger -Wcast-align -fno-short-double -ftoplevel-reorder -MMD  -mnewlib-libc -std=gnu++11 -mprocessor=32MZ2048EFG124 -DF_CPU=200000000UL  -DARDUINO=10609 -D_BOARD_OPENSCOPE_ -DMPIDEVER=16777998 -DMPIDE=150 -DIDE=Arduino -I${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32 -I${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/variants/OpenScope  -I.

CXXARFLAGS:=-g1 -O0 -Wa,--gdwarf-2 -mprocessor=32MZ2048EFG124 -DF_CPU=200000000UL  -DARDUINO=10609 -D_BOARD_OPENSCOPE_ -DMPIDEVER=16777998 -DMPIDE=150 -DIDE=Arduino -I${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32 -I${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/variants/OpenScope


all: AWG.o AnalogIn.o DCInstruments.o FeedBack.o LA.o OSMath.o TimeOutTmr9.o Trigger.o Version.o DHCP.o DNS.o HeapMgr.o ICMP.o IPStack.o InternetLayer.o LinkLayer.o SNTPv4.o System.o TCP.o TCPRFC793.o TCPStateMachine.o UDP.o ccsbcs.o fs_ff.o flash.o MRF24GAdaptor.o wf_connection_algorithm.o wf_connection_profile.o wf_data_msg.o wf_eint.o wf_eint_stub.o wf_event_queue.o wf_event_stub.o wf_gpio_stub.o wf_init.o wf_mgmt_msg.o wf_param_msg.o wf_pbkdf2.o wf_pll.o wf_power.o wf_raw.o wf_registers.o wf_scan.o wf_spi_stub.o wf_task.o wf_timer.o wf_timer_stub.o wf_ud_state.o Config.o GlobalData.o Helper.o IO.o Initialize.o LEDs.o LexJSON.o LoopStats.o MfgTest.o OSSerial.o ParseOpenScope.o ProcessJSONCmd.o WiFi.o DEIPcK.o TCPServer.o TCPSocket.o UDPServer.o UDPSocket.o DEWFcK.o DFATFS.o fs_diskio.o DMASerial.o DSDVOL.o DSPI.o FLASHVOL.o HTMLDefaultPage.o HTMLOptions.o HTMLPostCmd.o HTMLReboot.o HTMLSDPage.o HTTPHelpers.o ProcessClient.o ProcessServer.o deOpenScopeWebServer.o DEMRF24G.o Board_Data.o EFADC.o cpp-startup.S.o crti.S.o crtn.S.o pic32_software_reset.S.o vector_table.S.o HardwareSerial_cdcacm.o HardwareSerial_usb.o WInterrupts.o WSystem.o exceptions.o pins_arduino.o task_manager.o wiring.o wiring_analog.o wiring_digital.o wiring_pulse.o wiring_shift.o HardwareSerial.o IPAddress.o Print.o Stream.o Tone.o WMath.o WString.o doprnt.o main.o

%.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/variants/OpenScope/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/libraries/MRF24G/utility/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/libraries/MRF24G/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/libraries/FLASHVOL/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/libraries/DFATFS/utility/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/libraries/DEIPcK/utility/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


%.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

cpp-startup.S.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32/cpp-startup.S
	$(CXX) $(CXXARFLAGS) -c $< -o $@

crti.S.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32/crti.S
	$(CXX) $(CXXARFLAGS) -c $< -o $@

crtn.S.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32/crtn.S
	$(CXX) $(CXXARFLAGS) -c $< -o $@

pic32_software_reset.S.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32/pic32_software_reset.S
	$(CXX) $(CXXARFLAGS) -c $< -o $@

vector_table.S.o: ${HOME}/.arduino15/packages/Digilent/hardware/pic32/1.0.3/cores/pic32/vector_table.S
	$(CXX) $(CXXARFLAGS) -c $< -o $@

%.o: libraries/MRF24G/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: libraries/HTTPServer/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: libraries/FLASHVOL/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: libraries/DSPI/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: libraries/DSDVOL/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: libraries/DMASerial/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: libraries/DFATFS/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: libraries/DEWFcK/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: libraries/DEIPcK/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@