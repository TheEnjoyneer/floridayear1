# README.txt
# TLB: Team Let's Ball
# Term Project Code Instructions

Prerequisites:
1) SimpleScalar has been unpacked and installed in a local directory
2) SimpleScalar has been configured to run in Little Endian mode 
3) SimpleScalar has been configured to run PISA architecture binaries

Once the above prerequesites are taken care of, our simulation is very
simple to recreate with the supplied code in sim_configs and tests-pisa.

In our simulation testing, we executed 5 different precompiled binaries supplied 
by SimpleScalar in its installation package, which we have also supplied in
the tests-pisa/bin.little directory in our zip file, and we ran each binary
with each of our 6 different configuration files which are all supplied
in the sim_configs directory.

To actually execute the simulation, all that was done was running the following command
(with <config_file> and <test_binary> replaced with their corresponding values):
./sim-outorder -config <config_file> ./tests-pisa/bin.little/<test_binary>
