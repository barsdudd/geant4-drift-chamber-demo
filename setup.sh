#!/bin/bash
# Source this script to set up the runtime environment:
#   source setup.sh [/path/to/Geant4Data]
#
# The Geant4 dataset directory is resolved in this order:
#   1. the first argument to this script
#   2. an already-exported G4DATADIR
#   3. auto-detection via geant4-config (if it is on your PATH)

echo "=== Setting up Environment ==="

# 1. Visualization environment settings
# Respect an existing DISPLAY. If unset, ask launchd for the XQuartz display
# (macOS registers it there, e.g. /var/run/com.apple.launchd.*/org.xquartz:0);
# plain ":0" is only a last resort and is typically rejected by XQuartz.
if [ -z "$DISPLAY" ]; then
    command -v launchctl >/dev/null 2>&1 && DISPLAY=$(launchctl getenv DISPLAY)
    export DISPLAY="${DISPLAY:-:0}"
fi
export LIBGL_ALWAYS_SOFTWARE=1
export G4VIS_DEFAULT_DRIVER=TSGZB

# 2. Geant4 dataset path fixes (workaround for the gcosmo bug)
if [ -n "$1" ]; then
    G4DATADIR="$1"
elif [ -z "$G4DATADIR" ] && command -v geant4-config >/dev/null 2>&1; then
    # geant4-config --datasets prints: <name> <env-var> <path> ; all datasets
    # live in the same parent directory
    first_dataset=$(geant4-config --datasets 2>/dev/null | awk 'NR==1 {print $3}')
    [ -n "$first_dataset" ] && G4DATADIR=$(dirname "$first_dataset")
fi

if [ -z "$G4DATADIR" ] || [ ! -d "$G4DATADIR" ]; then
    echo "[!] Geant4 dataset directory not found (G4DATADIR='$G4DATADIR')."
    echo "    Usage: source setup.sh /path/to/Geant4Data"
    echo "    (the directory that contains G4EMLOW*, G4ENSDFSTATE*, etc.)"
    return 1 2>/dev/null || exit 1
fi
export G4DATADIR
echo "[-] Using Geant4 datasets in: $G4DATADIR"

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
