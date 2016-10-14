#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H
#include <memory>
#include <QVector>
#include <QString>
#include <QMap>
#include <mpi.h>
#include <lammps.h>
#include <fix.h>
#include <compute.h>
#include <modify.h>
#include <variable.h>
#include <error.h>
#include "scripthandler.h"
using namespace LAMMPS_NS;

struct LammpsState {
    bool crashed = false;
    bool runCommandActive = false;
    bool preRunNeeded = true;
    bool canProcessSimulatorControls = false;
    int  simulationSpeed = 1;
    bool dataDirty = false;
    unsigned long timeSpentInLammps = 0;
    unsigned int runCommandStart = 0;
    unsigned int runCommandEnd = 0;
    ScriptCommand nextCommand;
};

class LAMMPSController
{
private:
    ScriptHandler *m_scriptHandler = nullptr;
    LAMMPS *m_lammps = nullptr;
    class MyWorker *m_worker = nullptr;
    class System *m_system = nullptr;
    bool m_exceptionHandled = false;
    LAMMPSException m_currentException;
    void executeActiveRunCommand();

public:
    LammpsState state;
    class States *states = nullptr; // TODO: Naming

    LAMMPSController();
    ~LAMMPSController();

    void start();

    // Getters/setters
    QMap<QString, class SimulatorControl*> simulatorControls;
    LAMMPS *lammps() const;
    void setLammps(LAMMPS *lammps);
    void setWorker(class MyWorker *worker);
    int  simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    bool paused() const;
    void setPaused(bool value);
    bool crashed() const;
    LAMMPSException currentException();
    void disableAllEnsembleFixes();
    ScriptHandler* scriptHandler() const;
    void setScriptHandler(ScriptHandler* scriptHandler);
    class System *system() const;
    void setSystem(class System *system);

    // Actions
    void executeCommandInLAMMPS(QString command);
    void processCommand(QString command);
    void reset();
    bool tick();

    int findComputeId(QString identifier);
    int findVariableIndex(QString identifier);
    int findFixIndex(QString identifier);
    bool computeExists(QString identifier);
    bool fixExists(QString identifier);
    bool variableExists(QString identifier);
    bool groupExists(QString identifier);
    bool regionExists(QString identifier);
    LAMMPS_NS::Compute *findComputeByIdentifier(QString identifier);
    LAMMPS_NS::Fix *findFixByIdentifier(QString identifier);
    LAMMPS_NS::Variable *findVariableByIdentifier(QString identifier);
    template<class T>
    T *findFixByType();
    void processSimulatorControls();
    bool exceptionHandled() const;
    void setExceptionHandled(bool value);
};

template<class T>
T *LAMMPSController::findFixByType() {
    for(int i=0; i<m_lammps->modify->nfix; i++) {
        LAMMPS_NS::Fix *fix = m_lammps->modify->fix[i];

        T *myFix = dynamic_cast<T*>(fix);
        if(myFix) {
            return myFix;
        }
    }

    return nullptr;
}

#endif // LAMMPSCONTROLLER_H
