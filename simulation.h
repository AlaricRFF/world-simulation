#ifndef PROJECT_3_SIMULATION_H
#define PROJECT_3_SIMULATION_H
#define MAX_NUM_OPCODE 9
#define NO_ADDRESS 99;
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cassert>
#include <sstream>
#include "world_type.h"

using namespace std;
using std::string;


/// HELPER FUNCUTIONS
opcode_t string2Enum_instr(const string& instr);
direction_t string2Enum_direc(const string& direc);
void printWorldSquares(const world_t &world);
string crtStatus(const creature_t& crt);
void programIDUpdateN(creature_t& creature);
bool helper_facingOutsideBoundary(const world_t& world, const creature_t& creature);
bool helper_NotEnemy(const world_t& world, const creature_t& creature);
void creatureTakeAction_Print(world_t& world, creature_t& creature, const int &crtIndex);
void updateGridSquares(world_t& world);
void creatureActionConcise(world_t& world,creature_t& creature,const int& crtIndex);
/// INSTRUCTION OF SPECIES
void hop(creature_t &creature, world_t &world, const int &crtIndex);
void left(creature_t& creature);
void right(creature_t& creature);
void infect(world_t& world, creature_t &creature);
void ifempty(world_t &world, creature_t& creature);
void ifwall(const world_t& world,creature_t& creature);
void ifsame(const world_t& world,creature_t& creature);
void ifenemy(const world_t& world,creature_t& creature);
void go(creature_t& creature);

/// ERROR CHECKING


bool isLegal(const string& instr); // check input instruction is legal or not


/// read species
void readSpecies(const string &speciesSumFileName, unsigned int &speciesNum, species_t *speciesStoreArr);


// read creatures
bool isMatchSpec(species_t *speciesStoreArr, const unsigned int &speciesNum, creature_t &cur_creature,
                 const string &specName);
void
readWorld(creature_t (&creatureStoreArr)[MAXCREATURES], unsigned int &heightGrid, unsigned int &widthGrid, unsigned int &creatureNum,
          const string &worldName, species_t *speciesStoreArr, const unsigned int &speciesNum);




void
initWorld(world_t &world, const unsigned int &speciesNum, species_t *speciesStoreArr, const unsigned int &creatureNum,
          creature_t *creatureStoreArr, const unsigned int &width, const unsigned int &height);


// simulation
void verboseSimulation(world_t& world, const int& roundsTotal);
void conciseSimulation(world_t& world,const int& roundsTotal);


#endif //PROJECT_3_SIMULATION_H
