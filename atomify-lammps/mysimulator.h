#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <SimVis/CylinderData>
#include <QMap>
#include <QVector3D>
#include <QElapsedTimer>
#include <memory>
#include <cmath>
#include <mpi.h>
#include <lammps.h>
#include "lammpscontroller.h"
#include "LammpsWrappers/lammpserror.h"

using namespace LAMMPS_NS;

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    MyWorker();
    bool willPause() const;
    void setWillPause(bool willPause);

private:
    QElapsedTimer m_elapsed;
    QElapsedTimer m_sinceStart;
    LAMMPSController m_lammpsController;

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void work() override;
    bool m_willPause = false;
};

class AtomifySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(ScriptHandler* scriptHandler READ scriptHandler WRITE setScriptHandler NOTIFY scriptHandlerChanged)
    Q_PROPERTY(bool willReset READ willReset WRITE setWillReset NOTIFY willResetChanged)
    Q_PROPERTY(LammpsError* lammpsError READ lammpsError WRITE setLammpsError NOTIFY lammpsErrorChanged)
    Q_PROPERTY(bool automaticallyRun READ automaticallyRun WRITE setAutomaticallyRun NOTIFY automaticallyRunChanged)
    Q_PROPERTY(System* system READ system WRITE setSystem NOTIFY systemChanged)
public:
    AtomifySimulator();
    ~AtomifySimulator();

    // Simulator interface
    int simulationSpeed() const;
    bool paused() const;
    ScriptHandler* scriptHandler() const;
    bool willReset() const;
    Q_INVOKABLE void clearSimulatorControls();
    class System* system() const;
    LammpsError* lammpsError() const;
    bool automaticallyRun() const;

public slots:
    void setSimulationSpeed(int arg);
    void setPaused(bool paused);
    void setScriptHandler(ScriptHandler* scriptHandler);
    void setWillReset(bool willReset);
    void setSystem(class System* system);
    void setLammpsError(LammpsError* lammpsError);
    void setAutomaticallyRun(bool automaticallyRun);

signals:
    void simulationSpeedChanged(int arg);
    void pausedChanged(bool paused);
    void errorInLammpsScript();
    void lammpsDidReset();
    void scriptHandlerChanged(ScriptHandler* scriptHandler);
    void willResetChanged(bool willReset);
    void systemChanged(class System* system);
    void lammpsErrorChanged(LammpsError* lammpsError);
    void automaticallyRunChanged(bool automaticallyRun);

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    ScriptHandler* m_scriptHandler = nullptr;
    class System* m_system = nullptr;
    LammpsError* m_lammpsError = nullptr;
    LammpsState lammpsState;
    int m_simulationSpeed = 1;
    bool m_paused = false;
    bool m_willReset = false;
    bool m_automaticallyRun = false;
};

#endif // MYSIMULATOR_H
