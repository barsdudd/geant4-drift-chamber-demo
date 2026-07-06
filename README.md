# Geant4 Drift-Chamber Demo

A small Geant4 simulation of a fixed-target spectrometer, plus a standalone
ROOT macro that reconstructs tracks from the simulated drift-chamber hits.
Originally built as a lab-tour demonstration of how a Drell-Yan experiment
(SeaQuest/SpinQuest style) measures muon pairs.

## Layout

| Component | Position (z) | Description |
|---|---|---|
| Target | 0 m | Liquid hydrogen (G4_lH2), 10 x 10 x 40 cm |
| Beam dump | 3 m | Iron block, 1 x 1 x 4 m |
| Drift chambers | 5.5-5.7 m | 6 argon planes (U U' X X' V V'), 40 mm apart, stereo angle ±14° |

Two beam modes, switchable at runtime with a UI command:

- **Proton mode** (default): a single 50 GeV proton fired at the target,
  producing a realistic hadronic shower into the dump.
- **Drell-Yan mode**: a back-to-back 25 GeV mu+ / mu- pair generated at the
  target center, which punches through the dump and hits the chambers.

Chamber hits (plane ID, wire ID, drift distance smeared with 0.3 mm
resolution) are written to `ChamberHits.root`. Proton-primary events are not
recorded; only muon-pair events fill the tree.

## Requirements

- [Geant4](https://geant4.web.cern.ch/) 11.x with UI/visualization drivers
- [ROOT](https://root.cern/) 6.x
- CMake 3.16+

## Building

```bash
mkdir build && cd build
cmake ..            # add -DGeant4_DIR=... if Geant4 is not on your CMake path
make -j4
```

`vis.mac` is copied into `build/` automatically at configure time.

## Running the simulation

```bash
cd build
./DetectorDemo              # interactive: opens the viewer and loads vis.mac
```

In the UI session:

```
/demo/drellYanMode true     # switch to muon-pair mode (false = proton mode)
/run/beamOn 10
```

Or run in batch mode by passing a macro file:

```bash
./DetectorDemo run.mac
```

where `run.mac` contains, e.g.:

```
/run/initialize
/demo/drellYanMode true
/run/beamOn 100
```

`setup.sh` shows the environment variables (Geant4 dataset paths, default
visualization driver) used on the original machine; adapt the paths to your
installation if your Geant4 environment is not already configured.

## Track reconstruction

`tracking/TrackRecon.C` reads `build/ChamberHits.root` and the plane geometry
from `tracking/geometry.csv`, then:

1. pairs hits in adjacent planes (U/U', X/X', V/V'),
2. forms track candidates from X-V pair combinations and confirms them with
   the U planes,
3. resolves the left-right ambiguity by trying all 64 sign combinations of
   the 6 drift distances in a linear least-squares fit (`TLinearFitter`),
4. keeps tracks with chi2 < 30 and draws an event display per track.

```bash
cd tracking
root -l 'TrackRecon.C(true)'    # with event display (press Enter to advance)
root -l 'TrackRecon.C(false)'   # fit only, no display
```

`geometry.csv` columns: plane ID, z position (mm), wire pitch (mm), offset of
wire 0 (mm), stereo angle (degrees). It must match the geometry in
`DetectorConstruction.cc` and the wire mapping in `SteppingAction.cc`.

## License

Free to use for education and outreach.
