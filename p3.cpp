#include <iostream>
#include <string>
#include <cassert>
#include "world_type.h"
#include "simulation.h"

int main(int argc, char *argv[]) {
    /// ERROR CHECKING -- not enough input arguments
    if (argc < 4) {
        cout << "Error: Missing arguments!" << endl;
        cout << "Usage: ./p3 <species-summary> <world-file> <rounds> [v|verbose]" << endl;
        return 0;
    };
    string speciesSumFileName = argv[1];
    string worldFileName = argv[2];
    int roundsTotal = atoi(argv[3]);
    bool isVerbose = false;
    if (argc > 4) {
        if ((string)argv[4] == "v"  || (string)argv[4] ==  "verbose") {
            isVerbose = true;
        }
    }
    /// ERROR CHECKING -- round is negative
    if (roundsTotal < 0) {
        cout << "Error: Number of simulation rounds is negative!" << endl;
        return 0;
    }
    string speciesNameArr[MAXSPECIES]; // store all species' name
    unsigned int speciesNum = 0; // the species number
    species_t speciesStoreArr[MAXSPECIES]; // this is prepared for the initialization for the world_t
    unsigned int heightGrid = 0, widthGrid = 0, creatureNum = 0;
    creature_t creatureStoreArr[MAXCREATURES];
    world_t world;
    try {
        readSpecies(speciesSumFileName, speciesNum, speciesStoreArr);
        readWorld(creatureStoreArr, heightGrid, widthGrid, creatureNum, worldFileName, speciesStoreArr, speciesNum);
    }
    catch (string error){
        return 0;
    }
    initWorld(world, speciesNum, speciesStoreArr, creatureNum, creatureStoreArr, widthGrid, heightGrid);
    if (isVerbose) // decide the simulation mode
        verboseSimulation(world, roundsTotal);
    else
        conciseSimulation(world, roundsTotal);
    return 0;
}
