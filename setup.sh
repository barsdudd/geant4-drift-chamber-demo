#!/bin/bash

echo "=== Setting up Environment ==="

# 1. Visualization environment settings
export DISPLAY=:0
export LIBGL_ALWAYS_SOFTWARE=1
export G4VIS_DEFAULT_DRIVER=TSGZB

# 2. Geant4 dataset path fixes (workaround for the gcosmo bug)
# Note: assumes the datasets are located in the directory below
export G4DATADIR="/Users/knagai/Software/Geant4-11.4.0-Darwin/Geant4Data"

export G4ENSDFSTATEDATA=$(ls -d $G4DATADIR/G4ENSDFSTATE* 2>/dev/null)
export G4LEDATA=$(ls -d $G4DATADIR/G4EMLOW* 2>/dev/null)
export G4LEVELGAMMADATA=$(ls -d $G4DATADIR/PhotonEvaporation* 2>/dev/null)
export G4RADIOACTIVEDATA=$(ls -d $G4DATADIR/RadioactiveDecay* 2>/dev/null)
export G4PARTICLEXSDATA=$(ls -d $G4DATADIR/G4PARTICLEXS* 2>/dev/null)
export G4PIIDATA=$(ls -d $G4DATADIR/G4PII* 2>/dev/null)
export G4REALSURFACEDATA=$(ls -d $G4DATADIR/RealSurface* 2>/dev/null)
export G4SAIDXSDATA=$(ls -d $G4DATADIR/G4SAIDDATA* 2>/dev/null)
export G4ABLADATA=$(ls -d $G4DATADIR/G4ABLA* 2>/dev/null)
export G4INCLDATA=$(ls -d $G4DATADIR/G4INCL* 2>/dev/null)
export G4NEUTRONHPDATA=$(ls -d $G4DATADIR/G4NDL* 2>/dev/null)

echo "[-] Environment variables loaded."
