#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <QMap>
#include <vector>
#include "potential.h"
class QString;
class QIODevice;

namespace Langmuir
{
  /**
   * @struct SimulationParameters
   * @brief All pertinent simulation parameters are stored in here.
   */
  struct SimulationParameters
  {
    bool interactionCoulomb;
    bool chargedDefects;
    bool gaussianNoise;
    bool gridCharge;
    bool outputCarriers;
    bool outputCoulombPotential;
    bool outputFieldPotential;
    bool outputTrapsPotential;
    bool outputDefectIDs;
    bool outputTrapIDs;
    bool chargedTraps;
    bool trapsHeterogeneous;
    bool outputGrid;
    bool outputStats;
    bool useOpenCL;
    bool okCL;
    double chargePercentage;
    double defectPercentage;
    double deltaEpsilon;
    double gaussianAverg;
    double gaussianStdev;
    double seedPercentage;
    double sourceBarrier;
    double drainBarrier;
    double temperatureKelvin;
    double trapPercentage;
    double variableFinal;
    double variableStart;
    double voltageDrain;
    double voltageSource;
    double boltzmannConstant;
    double dielectricConstant;
    double elementaryCharge;
    double permittivitySpace;
    double gridFactor;
    double electrostaticPrefactor;
    double inverseKT;
    int electrostaticCutoff;
    int gridDepth;
    int gridHeight;
    int gridWidth;
    int iterationsPrint;
    int iterationsReal;
    int iterationsWarmup;
    int outputPrecision;
    int outputWidth;
    int potentialForm;
    int sourceType;
    int drainType;
    int variableSteps;
    int variableWorking;
    int zDefect;
    int zTrap;
    int hoppingRange;
    int workWidth;
    int workHeight;
    int workDepth;
    int workSize;
    int randomSeed;
    int sourceAttempts;
    QString kernelsPath;
    QVector< PotentialPoint > potentialPoints;

    SimulationParameters ()
    {
      interactionCoulomb = false;
      chargedDefects = false;
      gaussianNoise = false;
      gridCharge = false;
      chargedTraps = false;
      trapsHeterogeneous = false;
      outputGrid = false;
      outputStats = false;
      outputCarriers = false;
      outputCoulombPotential = false;
      useOpenCL = false;
      outputFieldPotential = false;
      outputTrapsPotential = false;
      outputDefectIDs = false;
      outputTrapIDs = false;
      chargePercentage = 0.01;
      defectPercentage = 0.00;
      deltaEpsilon = -0.10;
      gaussianAverg = 0.0;
      gaussianStdev = 0.0;
      seedPercentage = 0.0;
      sourceBarrier = 0.10;
      drainBarrier = 0.10;
      temperatureKelvin = 300.00;
      trapPercentage = 0.0;
      variableFinal = 0;
      variableStart = 0;
      voltageDrain = 0.00;
      voltageSource = 0.00;
      boltzmannConstant = 1.3806504e-23;
      dielectricConstant = 3.5;
      elementaryCharge = 1.60217646e-19;
      permittivitySpace = 8.854187817e-12;
      gridFactor = 1e-9;
      electrostaticCutoff = 50;
      gridDepth = 1;
      gridHeight = 256;
      gridWidth = 256;
      iterationsPrint = 1000;
      iterationsReal = 10000;
      iterationsWarmup = 1000;
      outputPrecision = 5;
      outputWidth = 20;
      potentialForm = 0;
      sourceType = 0;
      drainType = 0;
      variableSteps = 1;
      variableWorking = -1;
      zDefect = 0;
      zTrap = 0;
      hoppingRange = 1;
      workWidth = 4;
      workHeight = 4;
      workDepth = 4;
      workSize = 256;
      randomSeed = -1;
      kernelsPath = ".";
      sourceAttempts = 1;
      electrostaticPrefactor =
        elementaryCharge / (4.0 * M_PI * dielectricConstant *
                            permittivitySpace * gridFactor);
      inverseKT = 1.0 / (boltzmannConstant * temperatureKelvin);
    }
  };

  class InputParser
  {
  public:
    /**
     * @brief Constructor.
     *
     * Constructor. This is used to set up the Simulation instance, and saves
     * the parameters that should be varied in the simulation instance. These
     * parameters may be varied in serial fashion, or (TBC) distributed over the
     * cluster using MPI.
     *
     * @param fileName The name of the file to parse for simulation parameters.
     * parameters in the input file.
     */
    InputParser (const QString & fileName);

    /**
     * @enum The supported variables that can be varied - only one may be varied
     * in a simulation, although this could be extended to support more if the
     * need arises.
     */
    enum VariableType
    {
      e_undefined,
      e_interactionCoulomb,        // should Coulomb interaction be used
      e_chargedDefects,            // are the defects charged?
      e_gaussianNoise,             // add random noise to energy levels?
      e_gridCharge,                // seed the grid with charges
      e_chargedTraps,              // are the traps charged?
      e_trapsHeterogeneous,        // distrubute traps heterogeneously?
      e_outputXyz,                 // should trajectory files be written
      e_outputGrid,                // generate a pdf image of the grid?
      e_outputStats,               // output lifetime and pathlength to file?
      e_outputCarriers,            // output carrier positions to a file?
      e_outputCoulombPotential,    // output coulomb potential to file?
      e_outputFieldPotential,      // output field potential to file?
      e_outputTrapPotential,       // output trap potential to file?
      e_outputDefectIDs,           // output defect ids to file?
      e_outputTrapIDs,             // output trap ids to file?
      e_chargePercentage,          // percentage of charges in the grid - sets as target
      e_useOpenCL,                 // should OpenCL be used
      e_defectPercentage,          // percentage of defects in the grid
      e_deltaEpsilon,              // site energy difference for traps
      e_gaussianAverg,             // average of the random noise
      e_gaussianStdev,             // standard deviation of the random noise
      e_seedPercentage,            // percentage of trap seeds for heterogeneous traps
      e_sourceBarrier,             // probability to reject transport from source when sourceType = constant
      e_drainBarrier,              // probability to reject transport to drain when drainType = constant
      e_sourceAttempts,            // injection attempts by the source, if <= 0 then the source attempts current charges - max charges injections
      e_temperatureKelvin,         // the absolute temperature
      e_trapPercentage,            // percentage of traps in the grid
      e_variableFinal,             // final value of the variable range
      e_variableStart,             // the start of the variables range
      e_voltageDrain,              // voltage of the drain electrode
      e_voltageSource,             // voltage of the source electrode
      e_gridDepth,                 // the depth of the grid
      e_gridHeight,                // the height of the grid
      e_gridWidth,                 // the width of the grid
      e_iterationsPrint,           // number of iterations before printing state
      e_iterationsReal,            // the number of iterations for the real run
      e_iterationsWarmup,          // the number of warm up iterations to perform
      e_outputPrecision,           // decimal places to show in output
      e_outputWidth,               // number of char per output field
      e_potentialForm,             // how to calculate site energies ( linear )
      e_sourceType,                // how to determine the probability to inject charges from the source ( constant, coulomb, image )
      e_drainType,                 // how to determine the probability to accept charges at the drain ( constant, broke )
      e_variableSteps,             // the number of steps to take from start to final
      e_variableWorking,           // the working variable that is being changed
      e_zDefect,                   // charge on the defects (units e)
      e_zTrap,                     // charge on traps (units e)
      e_hoppingRange,              // number of neighbors to hop between
      e_workWidth,                 // local size of OpenCL work groups
      e_workHeight,                // local size of OpenCL work groups
      e_workDepth,                 // local size of OpenCL work groups
      e_workSize,                  // local size of OpenCL work groups
      e_randomSeed,                // seed for random number generator
      e_kernelsPath,               // source file containing OpenCL kernel
      e_potentialPoint,            // A point of defined potential (x,y,z,V)
      e_slopeZ,                    // convenience: makes two potential points at (0,0,1,slope.z) and (0,0,2,2*slope.z) - ( simulates gate potential )
      e_okCL,                      // can openCL be used on this platform?
      e_boltzmannConstant,         // thermodynamics constant - non-variable
      e_dielectricConstant,        // electrostatics constant - non-variable
      e_electrostaticCutoff,       // electrostatics constant - non-variable
      e_electrostaticPrefactor,    // collection of electrostatic constants - non-variable
      e_elementaryCharge,          // charge of electron - non-variable
      e_gridFactor,                // distance between centers of grid sites - non-variable
      e_inverseKT,                 // inverse average thermal energy - non-variable
      e_permittivitySpace,         // electrostatics constant - non-variable
      e_end
    };

    /**
     * @brief Get the simulation parameters for the supplied step.
     *
     * @param par The SimulationParameters that will be set up.
     * @param step The step
     * @param step Save the parameter pointer to m_parameters
     */
    bool simulationParameters ( SimulationParameters * par, int step = 0 );

    /**
     * Show the names and current values stored in par.
     * Shows current values in m_parameters if par = NULL.
     */
    QString printParameters ( SimulationParameters * par = NULL );

    /**
     * @brief access lines read.
     *
     * Get the number of valid parameters read in input file
     */
    int getReadCount();

  private:
    /**
     * @brief The values of parameters.
     *
     * Struct of variable parameters.
     */
      SimulationParameters m_parameters;

    /**
     * @brief string to variable type map
     *
     * Map of the input keys to the different simulation parameters
     */
    static QMap < QString, VariableType > s_variables;

    /**
     * @brief Line read.
     *
     * Process a line of input.
     */
    void processLine (QIODevice * file);

    /**
     * @brief set up map.
     *
     * Initialize the static map of variables if necessary
     */
    void initializeVariables ();

    /**
     * @brief lines read.
     *
     * Count the number of valid parameters read in input file
     */
    int readCount;

  };
}
#endif // INPUTPARSER_H
