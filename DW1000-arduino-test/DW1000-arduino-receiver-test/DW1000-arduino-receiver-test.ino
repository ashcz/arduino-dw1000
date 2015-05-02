/*
 * Copyright (c) 2015 by Thomas Trojer <thomas@trojer.net>
 * Decawave DW1000 library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Use this to test simple sender/receiver functionality with two
 * DW1000. Complements the "DW1000-arduino-sender-test" sketch. 
 */

#include <SPI.h>
#include <DW1000.h>

// DEBUG packet sent status and count
volatile boolean received = false;
volatile int numReceived = 0;
// reset line to the chip
int RST = 9;
// chip driver instances with chip select and reset
DW1000 dw = DW1000(SS, RST);

void setup() {
  // DEBUG monitoring
  Serial.begin(9600);
  // initialize the driver
  dw.initialize();
  Serial.println("DW1000 initialized ...");
  // general configuration
  dw.newConfiguration(); 
  dw.setDeviceAddress(6);
  dw.setNetworkId(10);
  dw.setFrameFilter(false);
  dw.interruptOnReceived(true);
  dw.commitConfiguration();
  Serial.println("Committed configuration ...");
  // DEBUG chip info and registers pretty printed
  Serial.print("Device ID: "); Serial.println(dw.getPrintableDeviceIdentifier());
  Serial.print("Unique ID: "); Serial.println(dw.getPrintableExtendedUniqueIdentifier());
  Serial.print("Network ID & Device Address: "); Serial.println(dw.getPrintableNetworkIdAndShortAddress());
  Serial.println(dw.getPrettyBytes(SYS_CFG, LEN_SYS_CFG));
  Serial.println(dw.getPrettyBytes(PANADR, LEN_PANADR));
  Serial.println(dw.getPrettyBytes(SYS_MASK, LEN_SYS_MASK));
  // attach interrupt and ISR
  pinMode(INT0, INPUT);
  attachInterrupt(0, serviceIRQ, FALLING);
  Serial.println("Interrupt attached ...");
  // configure as permanent receiver
  dw.newReceive();
  {
    dw.setDefaults();
    dw.startReceive();
  }
}

void serviceIRQ() {
  if(received) {
    return;
  }
  // "NOP" ISR
  received = true;
  numReceived++;
}

void loop() {
    // TODO proper sender config and receiver test
    // Interrupt version of transmit: Confirmation of ISR status change
    if(received) {
      Serial.print("Received packet ... #"); Serial.println(numReceived);
      Serial.println(dw.getDataLength());
      received = false;
      
      dw.newReceive();
      {
        dw.setDefaults();
        dw.startReceive();
      }
    }
    // wait a bit
    delay(100);
    //Serial.println(dw.getPrettyBytes(SYS_STATUS, LEN_SYS_STATUS));
}