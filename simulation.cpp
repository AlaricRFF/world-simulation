#include "simulation.h"

/// helper function

string crtStatus(const creature_t &crt) {
    // EFFECTS: return <species> <dir> <r> <c>. No empty space at either end
    return "(" + crt.species->name + " " + directName[crt.direction] + " " + to_string(crt.location.r) + " " +
           to_string(crt.location.c) + ")";
}

void printWorldSquares(const world_t &world) {
    // MODIFIES: cout
    // EFFECTS: print world.grid.square into cout
    for (unsigned int row = 0; row < world.grid.height; ++row) {
        for (unsigned int col = 0; col < world.grid.width; ++col) {
            if (world.grid.squares[row][col] == nullptr) {
                cout << "____ ";
            } else {
                cout << world.grid.squares[row][col]->species->name[0] << world.grid.squares[row][col]->species->name[1]
                     << "_"
                     << directShortName[world.grid.squares[row][col]->direction]
                     << ' ';
            }
        }
        cout << endl;
    }
}

bool isMatchSpec(species_t *speciesStoreArr, const unsigned int &speciesNum, creature_t &cur_creature,
                 const string &specName) {
    // EFFECTS: return true if the input species is found in the total species available, and link this creature to the species
    // MODIFIES: cur_creature' species state if return value is true
    for (unsigned int i = 0; i < speciesNum; ++i) {
        if (specName == speciesStoreArr[i].name) {
            cur_creature.species = &speciesStoreArr[i];
            return true;
        }
    }
    return false;
}

bool isLegal(const string &instr) {
    // EFFECTS: return true if the instr is one of the opcode_t name; false otherwise
    for (const auto &i: opName) {
        if (instr == i) {
            return true;
        }
    }
    return false;
}

direction_t string2Enum_direc(const string &direc) {
    /// ERROR CHECKING ATTACHED !
    // EFFECTS: if nothing goes wrong, return the direction_t of correlating direction;
    //          else, update error
    try {
        for (int i = 0; i < 4; ++i) {
            if (direc == directName[i]) {
                return (direction_t) i; // convert i into the direction_t enum
            }
        }
        throw -1; // let catch in the function to first detect the error
    }
    catch (int err) {
        /// ERROR CHECKING -- not matching direction
        cout << "Error: Direction " << direc << " is not recognized!" << endl;
        string error;
        error = "wrongDirection";
        throw error; // throw again and let the catch in the main to handle it
    }
}

opcode_t string2Enum_instr(const string &instr) {
    // REQUIRES: instr is in one of the opcode_t name
    // EFFECTS: convert a string to opcode_t with the same name
    try {
        if (!isLegal(instr)) {
            throw instr;
        }
        if (instr == "hop")
            return HOP;
        if (instr == "left")
            return LEFT;
        if (instr == "right")
            return RIGHT;
        if (instr == "infect")
            return INFECT;
        if (instr == "ifempty")
            return IFEMPTY;
        if (instr == "ifenemy")
            return IFENEMY;
        if (instr == "ifsame")
            return IFSAME;
        if (instr == "ifwall")
            return IFWALL;
        if (instr == "go")
            return GO;
        else {
            cout << "string2Num() & isLegal() needs fixing! " << endl;
            assert(0);
        }
    }
    catch (string error) {
        cout << error << " is not legal instruction" << endl;
        assert(0);
    }
}

void programIDUpdateN(creature_t &creature) {
    // MODIFIES: creature.programID
    // EFFECTS: programID add one if not exceeding programSize; otherwise reset it to 1
    if (creature.programID < creature.species->programSize)
        creature.programID++;
    else if (creature.programID == creature.species->programSize)
        creature.programID = 1;
    else {
        cerr << "error in updating programID! Exceed program size!" << endl;
        assert(0);
    }
}

bool helper_facingOutsideBoundary(const world_t &world, const creature_t &creature) {
    // REQUIRES: creature is inside the boundary
    // EFFECTS: return true if the square the creature is facing is at the boundary
    switch (creature.direction) {
        case EAST:
            return (creature.location.c == (int) world.grid.width - 1);
        case WEST:
            return (creature.location.c == 0);
        case NORTH:
            return (creature.location.r == 0);
        case SOUTH:
            return (creature.location.r == (int) world.grid.height - 1);
        default: {
            cout << "error in facingOutsideBoundary! Unknown direction!" << endl;
            assert(0);
        }
    }
}

bool helper_NotEnemy(const world_t &world, const creature_t &creature) {
    // EFFECTS: if the square the creature is facing is inside the grid and has a different species, return true(an empty grid is counted as the same species)
    if (helper_facingOutsideBoundary(world,
                                     creature)) // the square it's facing is out of the grid, return true, which means do nothing
        return true;
    switch (creature.direction) {
        case EAST: {
            if (world.grid.squares[creature.location.r][creature.location.c + 1] == nullptr)
                return true;
            if (world.grid.squares[creature.location.r][creature.location.c + 1]->species->name ==
                creature.species->name)
                return true;
            break;
        }
        case WEST: {
            if (world.grid.squares[creature.location.r][creature.location.c - 1] == nullptr)
                return true;
            if (world.grid.squares[creature.location.r][creature.location.c - 1]->species->name ==
                creature.species->name)
                return true;
            break;
        }
        case NORTH: {
            if (world.grid.squares[creature.location.r - 1][creature.location.c] == nullptr)
                return true;
            if (world.grid.squares[creature.location.r - 1][creature.location.c]->species->name ==
                creature.species->name)
                return true;
            break;
        }
        case SOUTH: {
            if (world.grid.squares[creature.location.r + 1][creature.location.c] == nullptr)
                return true;
            if (world.grid.squares[creature.location.r + 1][creature.location.c]->species->name ==
                creature.species->name)
                return true;
            break;
        }
        default: {
            cerr << "error in helper_NotEnemy, wrong direction" << endl;
            assert(0);
        }
    }
    return false;
}

void updateGridSquares(world_t &world) {
    // MODIFIES: world.grid.square
    // EFFECTS: after a creature HOP, reset square's pointer
    for (unsigned int row = 0; row < world.grid.height; ++row) {
        for (unsigned int col = 0; col < world.grid.width; ++col) {
            world.grid.squares[row][col] = nullptr;
        }
    }
    for (unsigned int crtIndex = 0; crtIndex < world.numCreatures; ++crtIndex) {
        world.grid.squares[world.creatures[crtIndex].location.r][world.creatures[crtIndex].location.c] = &world.creatures[crtIndex];
    }
}

/// INSTRUCTION OF SPECIES
void hop(creature_t& creature,world_t& world, const int &crtIndex){
    // MODIFIES: world.grid.square; the creature's position if it's not out of boundary or facing other creatures; creature.programID
    // EFFECTS: move forward one row/column and programID++
    switch (creature.direction) {
        case EAST:{
            if (creature.location.c < (int)world.grid.width - 1 &&
                world.grid.squares[creature.location.r][creature.location.c + 1] == nullptr){
                creature.location.c ++;
                break; // update the programID and re-initiate the world.grid.square
            }else{ // else, do nothing, just update the programID and return
                programIDUpdateN(creature);
                return;
            }
        }
        case WEST:{
            if (creature.location.c > 0 && world.grid.squares[creature.location.r][creature.location.c - 1] == nullptr){
                creature.location.c -- ;
                break;
            }else{
                programIDUpdateN(creature);
                return;
            }
        }
        case NORTH:{
            if (creature.location.r > 0 && world.grid.squares[creature.location.r - 1][creature.location.c] == nullptr){
                creature.location.r --;
                break;
            }else{
                programIDUpdateN(creature);
                return;
            }
        }
        case SOUTH:{
            if (creature.location.r <(int) world.grid.height - 1 &&
                world.grid.squares[creature.location.r + 1][creature.location.c] == nullptr){
                creature.location.r ++ ;
                break;
            }else{
                programIDUpdateN(creature);
                return;
            }
        }
        default:{
            cerr << "error in hop! Unknown direction!" << endl;
            assert(0);
        }
    }
    updateGridSquares(world);
    programIDUpdateN(creature);
}

void left(creature_t &creature) {
    // MODIFIES: the creature's direction; creature.programID
    // EFFECTS: turn left 90 degrees and programID++
    switch (creature.direction) {
        case EAST: {
            creature.direction = NORTH;
            break;
        }
        case NORTH: {
            creature.direction = WEST;
            break;
        }
        case WEST: {
            creature.direction = SOUTH;
            break;
        }
        case SOUTH: {
            creature.direction = EAST;
            break;
        }
    }
    programIDUpdateN(creature);
}

void right(creature_t &creature) {
    // MODIFIES: the creature's direction; creature.programID
    // EFFECTS: turn right 90 degrees and programID++
    switch (creature.direction) {
        case EAST: {
            creature.direction = SOUTH;
            break;
        }
        case SOUTH: {
            creature.direction = WEST;
            break;
        }
        case WEST: {
            creature.direction = NORTH;
            break;
        }
        case NORTH: {
            creature.direction = EAST;
            break;
        }
    }
    programIDUpdateN(creature);
}

void infect(world_t &world, creature_t &creature) {
    // MODIFIES: the species of the victim which is in the front of the "creature";  creature.programID++
    // EFFECTS: change the species of the victim as the same with the creature; if no creature in front, do nothing
    switch (creature.direction) {
        case EAST: {
            if (creature.location.c == (int) world.grid.width - 1) // first check whether it's out-of-bound
                break;
            if (world.grid.squares[creature.location.r][creature.location.c + 1] ==
                nullptr) // (r,c+1) is null_ptr, no creature in front
                break;
            if (world.grid.squares[creature.location.r][creature.location.c + 1]->species->name ==
                creature.species->name) // the species are of same type
                break;
            // if above are not satisfied, then modify the possibleCrt's info: species and programID
            world.grid.squares[creature.location.r][creature.location.c + 1]->species = creature.species;
            world.grid.squares[creature.location.r][creature.location.c + 1]->programID = 1;
            break;
        }
        case WEST: {
            if (creature.location.c == 0) // first check whether it's out-of-bound
                break;
            if (world.grid.squares[creature.location.r][creature.location.c - 1] ==
                nullptr) // (r,c-1) is null_ptr, no creature in front
                break;
            else if (world.grid.squares[creature.location.r][creature.location.c - 1]->species->name ==
                     creature.species->name) // the species are of same type
                break;
            // if above are not satisfied, then modify the possibleCrt's info: species and programID
            world.grid.squares[creature.location.r][creature.location.c - 1]->species = creature.species;
            world.grid.squares[creature.location.r][creature.location.c - 1]->programID = 1;
            break;
        }
        case NORTH: {
            if (creature.location.r == 0) // first check whether it's out-of-bound
                break;
            if (world.grid.squares[creature.location.r - 1][creature.location.c] ==
                nullptr) // (r-1,c) is null_ptr, no creature in front
                break;
            else if (world.grid.squares[creature.location.r - 1][creature.location.c]->species->name ==
                     creature.species->name) // the species are of same type
                break;
            // if above are not satisfied, then modify the possibleCrt's info: species and programID
            world.grid.squares[creature.location.r - 1][creature.location.c]->species = creature.species;
            world.grid.squares[creature.location.r - 1][creature.location.c]->programID = 1;
            break;
        }
        case SOUTH: {
            if (creature.location.r == (int) world.grid.height - 1) // first check whether it's out-of-bound
                break;
            if (world.grid.squares[creature.location.r + 1][creature.location.c] ==
                nullptr) // (r+1,c) is null_ptr, no creature in front
                break;
            else if (world.grid.squares[creature.location.r + 1][creature.location.c]->species->name ==
                     creature.species->name) // the species are of same type
                break;
            // if above are not satisfied, then modify the possibleCrt's info: species and programID
            world.grid.squares[creature.location.r + 1][creature.location.c]->species = creature.species;
            world.grid.squares[creature.location.r + 1][creature.location.c]->programID = 1;
            break;
        }
        default: {
            cerr << "error in infect!" << endl;
            assert(0);
        }
    }
    programIDUpdateN(creature); // update programID
}

void ifempty(world_t &world, creature_t &creature) {
    // MODIFIES: creature.programID
    // EFFECTS: if the square in front is empty and inside the grid, change programID to n; otherwise, programID++
    if (creature.direction == EAST) {
        if (creature.location.c == (int) world.grid.width - 1) { // out-of-boundary
            programIDUpdateN(creature);
            return;
        }
        if (world.grid.squares[creature.location.r][creature.location.c + 1] != nullptr) { // square not empty
            programIDUpdateN(creature);
            return;
        }
        // condition satisfied, go to step n, which is stored in the current program[ID - 1].address
        creature.programID = creature.species->program[creature.programID - 1].address;
    }
    if (creature.direction == WEST) {
        if (creature.location.c == 0) { // out-of-boundary
            programIDUpdateN(creature);
            return;
        }
        if (world.grid.squares[creature.location.r][creature.location.c - 1] != nullptr) { // square not empty
            programIDUpdateN(creature);
            return;
        }
        creature.programID = creature.species->program[creature.programID - 1].address;
    }
    if (creature.direction == NORTH) {
        if (creature.location.r == 0) { // out-of-boundary
            programIDUpdateN(creature);
            return;
        }
        if (world.grid.squares[creature.location.r - 1][creature.location.c] != nullptr) { // square not empty
            programIDUpdateN(creature);
            return;
        }
        creature.programID = creature.species->program[creature.programID - 1].address;
    }
    if (creature.direction == SOUTH) {
        if (creature.location.r == (int) world.grid.height - 1) { // out-of-boundary
            programIDUpdateN(creature);
            return;
        }
        if (world.grid.squares[creature.location.r + 1][creature.location.c] != nullptr) { // square not empty
            programIDUpdateN(creature);
            return;
        }
        creature.programID = creature.species->program[creature.programID - 1].address;
    }
}

void ifwall(const world_t &world, creature_t &creature) {
    // MODIFIES: creature.programID
    // EFFECTS: if the square in front is the border, change programID to n; otherwise, programID++
    switch (creature.direction) {
        case EAST: {
            if (creature.location.c == (int) world.grid.width - 1) {
                creature.programID = creature.species->program[creature.programID - 1].address;
                return;
            }
            break;
        }
        case WEST: {
            if (creature.location.c == 0) {
                creature.programID = creature.species->program[creature.programID - 1].address;
                return;
            }
            break;
        }
        case NORTH: {
            if (creature.location.r == 0) {
                creature.programID = creature.species->program[creature.programID - 1].address;
                return;
            }
            break;
        }
        case SOUTH: {
            if (creature.location.r == (int) world.grid.height - 1) {
                creature.programID = creature.species->program[creature.programID - 1].address;
                return;
            }
            break;
        }
        default: {
            cerr << "error in ifwall, direction not found!" << endl;
            assert(0);
        }
    }
    programIDUpdateN(creature);
    // if the switch block is not returned, then the facing wall condition is not reached, meaning normally update ID
}

void ifsame(const world_t &world, creature_t &creature) {
    // MODIFIES: creature.programID
    // EFFECTS: if the square in front is the border, change programID to n; otherwise, programID++
    switch (creature.direction) {
        case EAST: {
            if (creature.location.c == (int) world.grid.width - 1) // out-of-boundary
                break;
            if (world.grid.squares[creature.location.r][creature.location.c + 1] == nullptr) // no creature
                break;
            if (world.grid.squares[creature.location.r][creature.location.c + 1]->species->name ==
                creature.species->name) { // only same species type
                creature.programID = creature.species->program[creature.programID - 1].address;
                return; // programID upgraded, exit
            }
            else
                break;
        }
        case WEST: {
            if (creature.location.c == 0)
                break;
            if (world.grid.squares[creature.location.r][creature.location.c - 1] == nullptr)
                break;
            if (world.grid.squares[creature.location.r][creature.location.c - 1]->species->name ==
                creature.species->name){
                creature.programID = creature.species->program[creature.programID - 1].address;
                return;
            }
            else
                break;
        }
        case SOUTH: {
            if (creature.location.r == (int)world.grid.height - 1)
                break;
            if (world.grid.squares[creature.location.r + 1][creature.location.c] == nullptr)
                break;
            if (world.grid.squares[creature.location.r + 1][creature.location.c]->species->name ==
                creature.species->name){
                creature.programID = creature.species->program[creature.programID - 1].address;
                return;
            }
            else
                break;
        }
        case NORTH: {
            if (creature.location.r == 0)
                break;
            if (world.grid.squares[creature.location.r - 1][creature.location.c] == nullptr)
                break;
            if (world.grid.squares[creature.location.r - 1][creature.location.c]->species->name ==
                creature.species->name) {
                creature.programID = creature.species->program[creature.programID - 1].address;
                return;
            }
            else
                break;
        }
        default: {
            cerr << "error in ifsame! Unknown direction!" << endl;
            assert(0);
        }
    }
    programIDUpdateN(creature);
}


void ifenemy(const world_t &world, creature_t &creature) {
    // MODIFIES: creature.programID
    // EFFECTS: if the square in front is the border, change programID to n; otherwise, programID++
    if (!helper_NotEnemy(world, creature)) { // the different species
        creature.programID = creature.species->program[creature.programID - 1].address;
    } else {
        programIDUpdateN(creature);
    }

}

void go(creature_t &creature) {
    // MODIFIES: creature.programID
    // EFFECTS: creature.programID = address
    creature.programID = creature.species->program[creature.programID - 1].address;
}

/// read species
void readSpecies(const string &speciesSumFileName, unsigned int &speciesNum, species_t *speciesStoreArr) {
    // REQUIRES: speciesNum is initially set to zero
    // MODIFIES: speciesNum, speciesStoreArr
    // EFFECTS: read the species information into the array of species_t
    /// TEMPORARY VARIABLE
    string tempStr_reuse; // store all the temporary string element. Always initialize it with "string()" before using
    ifstream tempIfstream_reuse;
    /*    Input Reading Starts   */
    /// @P1: read the species summary
    /// variable declaration begins (VDB)
    ifstream speciesSummary;
    speciesSummary.open(speciesSumFileName);
    /// ERROR CHECKING -- cannot open file
    if (!speciesSummary) {
        cout << "Error: Cannot open file " << speciesSumFileName << "!" << endl;
        speciesSummary.close(); // close the file before throwing
        string error = "fileOpenErr";
        throw error;
    }
    string speProgramDir; // store creature files' directory
    string speciesNameArr[MAXSPECIES]; // store all species' name
    /// variable declaration ends (VDE)
    getline(speciesSummary, speProgramDir); // first, store the directory of all program files
    tempStr_reuse = string(); // store the current species name
    // read species name into speciesNameArr and count the number of the species
    while (getline(speciesSummary,tempStr_reuse) && !tempStr_reuse.empty()){
        /// ERROR CHECKING -- exceeded species number
        if (speciesNum >= MAXSPECIES) {
            string error;
            error = "tooManySpecies";
            cout << "Error: Too many species!" << endl;
            cout << "Maximal number of species is " << MAXSPECIES << "." << endl;
            speciesSummary.close(); // close the file before throwing
            throw error;
        }
        speciesNameArr[speciesNum] = tempStr_reuse;
        speciesNum ++;
    }
    speciesSummary.close();
    /// @P2: read species files into an array species_t
    tempIfstream_reuse.clear(); // store the current species' information
    for (unsigned int curSpeciesIndex = 0;
         curSpeciesIndex < speciesNum; ++curSpeciesIndex) { // read species one by one into the speStore array
        /// VDB
        tempIfstream_reuse.clear(); // make empty
        string line = string(); // make empty
        string filename_temp = string(); // make empty
        speciesStoreArr[curSpeciesIndex].name = speciesNameArr[curSpeciesIndex];
        /// VDE
        filename_temp = speProgramDir + "/" + speciesNameArr[curSpeciesIndex];
        tempIfstream_reuse.open(filename_temp);
        /// ERROR CHECKING -- cannot open file
        if (!tempIfstream_reuse) {
            string error;
            error = "fileOpenErr";
            tempIfstream_reuse.close(); // close the file before throwing
            cout << "Error: Cannot open file " << filename_temp << "!" << endl;
            throw error;
        }
        /// INPUT PROCESSING -- read the current species' program -- handling @comments, @blank_line and @arguments
        /// VDB
        unsigned int programSize = 0;
        string instructionName;
        /// VDE
        // read species program, starting from first line
        while (getline(tempIfstream_reuse,line) && !line.empty()) {  // reading empty line, break and terminate the reading
            /// ERROR CHECKING -- programSize exceeded
            if (programSize >= MAXPROGRAM) {
                string error;
                error = "exceededProgramSize";
                cout << "Error: Too many instructions for species " << speciesStoreArr[curSpeciesIndex].name << "!"
                     << endl;
                cout << "Maximal number of instructions is " << MAXPROGRAM << "." << endl;
                tempIfstream_reuse.close(); // close the file before throwing
                throw error;
            }
            stringstream ss;
            ss << line;
            ss >> instructionName;
            /// ERROR CHECKING -- illegal instruction
            if (!isLegal(instructionName)) {
                string error;
                error = "UnknownInstruction";
                cout << "Error: Instruction " << instructionName << " is not recognized!" << endl;
                tempIfstream_reuse.close(); // close the files before the exception happens
                throw error;
            }
            if (instructionName.substr(0, 2) == "if" || instructionName.substr(0, 2) == "go") {
                // function with multiple argument
                speciesStoreArr[curSpeciesIndex].program[programSize].op = string2Enum_instr(instructionName);
                unsigned int temp_address;
                ss >> temp_address;
                speciesStoreArr[curSpeciesIndex].program[programSize].address = (unsigned int) temp_address;
            } else {
                speciesStoreArr[curSpeciesIndex].program[programSize].op = string2Enum_instr(instructionName);
                speciesStoreArr[curSpeciesIndex].program[programSize].address = NO_ADDRESS;
            }
            programSize++; // after finishing reading this line, increment the program size
        }
        speciesStoreArr[curSpeciesIndex].programSize = programSize;
        tempIfstream_reuse.close();
    }
}

/// read creatures
void
readWorld(creature_t (&creatureStoreArr)[MAXCREATURES], unsigned int &heightGrid, unsigned int &widthGrid,
          unsigned int &creatureNum,
          const string &worldName, species_t *speciesStoreArr, const unsigned int &speciesNum) {
    // REQUIRES: heightGrid, widthGrid and creatureNum should be initially set to zero
    // EFFECTS: store all creature information into creatureStoreArr, also the height and width of the grid and creature number
    // MODIFIES: creatureStoreArr, adding new creature into it; heightGrid, widthGrid, creatureNum
    ifstream worldFile;
    worldFile.open(worldName);
    /// ERROR CHECKING -- cannot open file
    if (!worldFile) {
        string error;
        error = "fileOpenErr";
        worldFile.close(); // close the file before throwing
        cout << "Error: Cannot open file " << worldName << "!" << endl;
        throw error;
    }
    string line = string();
    stringstream ss;
    /// grid height
    getline(worldFile, line);
    ss << line;
    ss >> heightGrid;
    /// grid width
    getline(worldFile, line);
    ss.clear();
    ss << line;
    ss >> widthGrid;
    /// ERROR CHECKING -- grid height || width
    if (heightGrid < 1 || heightGrid > MAXHEIGHT) {
        string error;
        error = "gridHeightErr";
        worldFile.close();
        cout << "Error: The grid height is illegal!" << endl;
        throw error;
    }
    if (widthGrid < 1 || widthGrid > MAXWIDTH) {
        string error;
        error = "gridWidthErr";
        worldFile.close();
        cout << "Error: The grid width is illegal!" << endl;
        throw error;
    }
    /// read creature information
    while (getline(worldFile,line) && !line.empty()) { // with a turn reads one creature's info
        const string& creature_original_row_description = line;
        ss.clear(); // clean the stringstream object
        ss << line;
        // input species name
        string creature_spec = string();
        ss >> creature_spec;
        if (creature_spec.empty()) break; // double check that the input line is not empty
        /// ERROR CHECKING -- creature number exceeded
        if (creatureNum >= MAXCREATURES) {
            string error;
            error = "creatureNumExceeded";
            worldFile.close();
            cout << "Error: Too many creatures!" << endl;
            cout << "Maximal number of creatures is " << MAXCREATURES << "." << endl;
            throw error;
        }
        /// ERROR CHECKING -- species matching current ones
        if (!isMatchSpec(speciesStoreArr, speciesNum, creatureStoreArr[creatureNum], creature_spec)) {
            cout << "Error: Species " << creature_spec << " not found!" << endl;
            worldFile.close();
            string error;
            error = "speciesNotFound";
            throw error;
        }
        /// isMatchSpec(speciesStoreArr, speciesNum, creatureStoreArr[creatureNum]) has already modified the creature, if the assertion doesn't fail
        // input creature's direction
        string temp_direc;
        ss >> temp_direc;
        /// ERROR CHECKING -- wrong direction
        creatureStoreArr[creatureNum].direction = string2Enum_direc(temp_direc);
        // input creature's position
        ss >> creatureStoreArr[creatureNum].location.r >> creatureStoreArr[creatureNum].location.c;
        /// ERROR CHECKING -- inside boundary or not
        if (creatureStoreArr[creatureNum].location.r < 0 ||
            creatureStoreArr[creatureNum].location.r >= (int) heightGrid ||
            creatureStoreArr[creatureNum].location.c < 0 ||
            creatureStoreArr[creatureNum].location.c >= (int) widthGrid) {
            cout << "Error: Creature (" << creature_original_row_description << ") is out of bound!" << endl;
            cout << "The grid size is " << heightGrid << "-by-" << widthGrid << "." << endl;
            worldFile.close();
            string error;
            error = "outsideBoundary";
            throw error;
        }
        creatureStoreArr[creatureNum].programID = 1;
        creatureNum++;

    }
    /// ERROR CHECKING -- creatures overlapping
    for (unsigned int i = 0; i < creatureNum; ++i) {
        for (unsigned int j = i + 1; j < creatureNum; ++j) {
            if (creatureStoreArr[i].location.c == creatureStoreArr[j].location.c &&
                creatureStoreArr[i].location.r == creatureStoreArr[j].location.r) {
                string spec_second =
                        "(" + creatureStoreArr[j].species->name + " " + directName[creatureStoreArr[j].direction] +
                        " " +
                        to_string(creatureStoreArr[j].location.r) + " " + to_string(creatureStoreArr[j].location.c) +
                        ") ";
                string spec_first =
                        "(" + creatureStoreArr[i].species->name + " " + directName[creatureStoreArr[i].direction] +
                        " " +
                        to_string(creatureStoreArr[i].location.r) + " " + to_string(creatureStoreArr[i].location.c) +
                        ")";
                cout << "Error: Creature " << spec_second << "overlaps with creature " << spec_first << "!" << endl;
                string error;
                error = "creatureOverlap";
                worldFile.close();
                throw error;
            }
        }
    }
    worldFile.close();
}

//simulation
void
initWorld(world_t &world, const unsigned int &speciesNum, species_t *speciesStoreArr, const unsigned int &creatureNum,
          creature_t *creatureStoreArr, const unsigned int &width, const unsigned int &height) {
    // MODIFIES: world
    // EFFECTS: fill the struct world with all required information
    world.numSpecies = speciesNum;
    world.numCreatures = creatureNum;
    for (unsigned int i = 0; i < speciesNum; ++i) {
        world.species[i] = speciesStoreArr[i];
    }
    for (unsigned int i = 0; i < creatureNum; ++i) {
        world.creatures[i] = creatureStoreArr[i];
    }
    world.grid.height = height;
    world.grid.width = width;
    for (unsigned int row = 0; row < height; ++row) {
        for (unsigned int col = 0; col < width; ++col) {
            world.grid.squares[row][col] = nullptr; // if the square is empty, then it points to a NULL pointer
        }
    }
    for (unsigned int i = 0; i < creatureNum; ++i) {
        world.grid.squares[creatureStoreArr[i].location.r][creatureStoreArr[i].location.c] = &world.creatures[i];// let each square points to its owner creature
    }


}

void creatureTakeAction_Print(world_t &world, creature_t &creature, const int &crtIndex) {
    // MODIFIES: world, creature, cout
    // EFFECTS: creature takes one action and print its action to cout
    cout << "Instruction " << creature.programID << ": "
         << opName[creature.species->program[creature.programID - 1].op];
    if (opName[creature.species->program[creature.programID - 1].op].substr(0, 2) == "if" ||
        opName[creature.species->program[creature.programID - 1].op].substr(0, 2) == "go") {
        cout << " " << creature.species->program[creature.programID - 1].address << endl;
    } else {
        cout << endl;
    }
    switch (creature.species->program[creature.programID - 1].op) {
        case HOP: {
            hop(creature, world, crtIndex);
            break;
        }
        case LEFT: {
            left(creature);
            break;
        }
        case RIGHT: {
            right(creature);
            break;
        }
        case INFECT: {
            infect(world, creature);
            break;
        }
        case IFEMPTY: {
            ifempty(world, creature);
            break;
        }
        case IFWALL: {
            ifwall(world, creature);
            break;
        }
        case IFSAME: {
            ifsame(world, creature);
            break;
        }
        case IFENEMY: {
            ifenemy(world, creature);
            break;
        }
        case GO: {
            go(creature);
            break;
        }
        default: {
            cerr << "error in creatureTakeAction_Print, unknown instruction!" << endl;
            assert(0);
        }
    }
    // after finishing executing the instruction, print out the message to cout
}

void creatureActionConcise(world_t &world, creature_t &creature, const int &crtIndex) {
    // MODIFIES: world, creature, cout
    // EFFECTS: creature takes one action and print to cout. However, in concise mode
    if (creature.species->program[creature.programID - 1].op == HOP ||
        creature.species->program[creature.programID - 1].op == LEFT ||
        creature.species->program[creature.programID - 1].op == RIGHT ||
        creature.species->program[creature.programID - 1].op == INFECT) {
        cout << "Creature " << crtStatus(creature) << " takes action: "
             << opName[creature.species->program[creature.programID - 1].op] << endl;
    }
    switch (creature.species->program[creature.programID - 1].op) {
        case HOP: {
            hop(creature, world, crtIndex);
            break;
        }
        case LEFT: {
            left(creature);
            break;
        }
        case RIGHT: {
            right(creature);
            break;
        }
        case INFECT: {
            infect(world, creature);
            break;
        }
        case IFEMPTY: {
            ifempty(world, creature);
            break;
        }
        case IFWALL: {
            ifwall(world, creature);
            break;
        }
        case IFSAME: {
            ifsame(world, creature);
            break;
        }
        case IFENEMY: {
            ifenemy(world, creature);
            break;
        }
        case GO: {
            go(creature);
            break;
        }
        default: {
            cerr << "error in creatureActionConcise, unknown instruction!" << endl;
            assert(0);
        }
    }
}

void verboseSimulation(world_t &world, const int &roundsTotal) {
    // MODIFIES: world
    // EFFECTS: simulate the verbose output of the world until roundsTotal is met 
    // all the instruction is encapsulated with programID update
    cout << "Initial state" << endl;
    printWorldSquares(world);
    // initial state
    for (int curRnd = 1; curRnd <= roundsTotal; ++curRnd) {
        // print round
        cout << "Round " << curRnd << endl;
        for (unsigned int curCrtIndex = 0; curCrtIndex < world.numCreatures; ++curCrtIndex) {
            creature_t &creature = world.creatures[curCrtIndex]; // safe some typing
            cout << "Creature " << crtStatus(creature) << " takes action:" << endl;
            while (true) {
                if (opName[creature.species->program[creature.programID - 1].op].substr(0, 2) == "if" ||
                    opName[creature.species->program[creature.programID - 1].op].substr(0, 2) == "go") {
                    creatureTakeAction_Print(world, creature, curCrtIndex);
                } else { // only takes one instruction
                    creatureTakeAction_Print(world, creature, curCrtIndex);
                    break;
                }
            }
            printWorldSquares(world);
        }
    }
}

void conciseSimulation(world_t &world, const int &roundsTotal) {
    // MODIFIES: world
    // EFFECTS: simulate the concise output of the world until roundsTotal is met
    // all the instruction is encapsulated with programID update
    cout << "Initial state" << endl;
    printWorldSquares(world);
    // initial state
    for (int curRnd = 1; curRnd <= roundsTotal; ++curRnd) {
        // print round
        cout << "Round " << curRnd << endl;
        for (unsigned int curCrtIndex = 0; curCrtIndex < world.numCreatures; ++curCrtIndex) {
            creature_t &creature = world.creatures[curCrtIndex]; // safe some typing
            while (true) {
                if (opName[creature.species->program[creature.programID - 1].op].substr(0, 2) == "if" ||
                    opName[creature.species->program[creature.programID - 1].op].substr(0, 2) == "go") {
                    creatureActionConcise(world, creature, curCrtIndex);
                } else { // only takes one instruction
                    creatureActionConcise(world, creature, curCrtIndex);
                    break;
                }
            }
        }
        // print the world at the end of each round
        printWorldSquares(world);
    }
}


