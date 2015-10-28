#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <QMap>
#include <QElapsedTimer>
#include <memory>
#include <cmath>
#include <QVector3D>
#include <mpi.h>
#include <lammps.h>

#include "lammpscontroller.h"
#include "atomstyle.h"
#include "scripthandler.h"

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
    virtual void synchronizeRenderer(Renderable *renderableObject) override;
    virtual void work() override;
    bool m_willPause = false;
    AtomStyle m_atomStyle;
    QVector<int> m_atomTypes;
};

class AtomifySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(double simulationTime READ simulationTime WRITE setSimulationTime NOTIFY simulationTimeChanged)
    Q_PROPERTY(int numberOfTimesteps READ numberOfTimesteps WRITE setNumberOfTimesteps NOTIFY numberOfTimestepsChanged)
    Q_PROPERTY(int numberOfAtoms READ numberOfAtoms WRITE setNumberOfAtoms NOTIFY numberOfAtomsChanged)
    Q_PROPERTY(int numberOfAtomTypes READ numberOfAtomTypes WRITE setNumberOfAtomTypes NOTIFY numberOfAtomTypesChanged)
    Q_PROPERTY(QVector3D systemSize READ systemSize WRITE setSystemSize NOTIFY systemSizeChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(AtomStyle* atomStyle READ atomStyle WRITE setAtomStyle NOTIFY atomStyleChanged)
    Q_PROPERTY(double timePerTimestep READ timePerTimestep WRITE setTimePerTimestep NOTIFY timePerTimestepChanged)
    Q_PROPERTY(QString lammpsError READ lammpsError WRITE setLammpsError NOTIFY lammpsErrorChanged)
    Q_PROPERTY(QString lammpsErrorMessage READ lammpsErrorMessage WRITE setLammpsErrorMessage NOTIFY lammpsErrorMessageChanged)
    Q_PROPERTY(ScriptHandler* scriptHandler READ scriptHandler WRITE setScriptHandler NOTIFY scriptHandlerChanged)
    Q_PROPERTY(bool willReset READ willReset WRITE setWillReset NOTIFY willResetChanged)
public:
    AtomifySimulator() { }
    ~AtomifySimulator() { }

    // Simulator interface
    int simulationSpeed() const;
    void addCompute(CPCompute *compute);
    QMap<QString, CPCompute *> computes() const;
    void setComputes(const QMap<QString, CPCompute *> &computes);
    AtomStyle* atomStyle() const;
    double simulationTime() const;
    bool paused() const;
    int numberOfAtoms() const;
    int numberOfAtomTypes() const;
    QVector3D systemSize() const;
    double timePerTimestep() const;
    QString lammpsError() const;
    QString lammpsErrorMessage() const;
    ScriptHandler* scriptHandler() const;
    bool willReset() const;
    int numberOfTimesteps() const;

public slots:
    void setSimulationSpeed(int arg);
    void setPaused(bool paused);
    void setSimulationTime(double simulationTime);
    void setAtomStyle(AtomStyle* atomStyle);
    void setNumberOfAtoms(int numberOfAtoms);
    void setNumberOfAtomTypes(int numberOfAtomTypes);
    void setSystemSize(QVector3D systemSize);
    void setTimePerTimestep(double timePerTimestep);
    void setLammpsError(QString lammpsError);
    void setLammpsErrorMessage(QString lammpsErrorMessage);
    void setScriptHandler(ScriptHandler* scriptHandler);
    void setWillReset(bool willReset);
    void setNumberOfTimesteps(int numberOfTimesteps);

signals:
    void simulationSpeedChanged(int arg);
    void pausedChanged(bool paused);
    void simulationTimeChanged(double simulationTime);
    void atomStyleChanged(AtomStyle* atomStyle);
    void numberOfAtomsChanged(int numberOfAtoms);
    void numberOfAtomTypesChanged(int numberOfAtomTypes);
    void systemSizeChanged(QVector3D systemSize);
    void timePerTimestepChanged(double timePerTimestep);
    void errorInLammpsScript();
    void lammpsErrorChanged(QString lammpsError);
    void lammpsErrorMessageChanged(QString lammpsErrorMessage);
    void lammpsDidReset();
    void scriptHandlerChanged(ScriptHandler* scriptHandler);
    void willResetChanged(bool willReset);
    void numberOfTimestepsChanged(int numberOfTimesteps);

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    int m_simulationSpeed = 1;
    bool m_paused = false;
    QMap<QString, CPCompute*> m_computes;
    double m_simulationTime = 0.0;
    AtomStyle* m_atomStyle = nullptr;
    int m_numberOfAtoms;
    int m_numberOfAtomTypes;
    QVector3D m_systemSize;
    double m_timePerTimestep = 0;
    QString m_lammpsError;
    QString m_lammpsErrorMessage;
    ScriptHandler* m_scriptHandler = nullptr;
    bool m_willReset = false;
    int m_numberOfTimesteps = 0;
};

#endif // MYSIMULATOR_H
