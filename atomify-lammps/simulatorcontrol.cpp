#include "simulatorcontrol.h"
#include "lammpscontroller.h"
#include "scripthandler.h"
#include "mysimulator.h"

SimulatorControl::SimulatorControl(QObject *parent) : QObject(parent)
{

}

bool SimulatorControl::addToLammps(LAMMPSController *lammpsController) {
    qDebug() << "Trying to add to lammps...";
    for(const QVariant &variant : m_dependencies) {
        SimulatorControl *dependency = qvariant_cast<SimulatorControl*>(variant);
        if(dependency && !dependency->existsInLammps(lammpsController)) {
            // We found one dependency that is not added to LAMMPS, abort this mission
            return false;
        }
    }

    qDebug() << "All dependencies are met";
    ScriptHandler *scriptHandler = lammpsController->scriptHandler();
    scriptHandler->addCommandToTop(ScriptCommand(enabledCommand(), ScriptCommand::Type::SingleCommand));
    return true;
}

bool SimulatorControl::dependenciesValid(LAMMPSController *lammpsController)
{
    bool valid = true;
    for(const QVariant &variant : m_dependencies) {
        SimulatorControl *dependency = qvariant_cast<SimulatorControl*>(variant);
        if(dependency) {
            // Check if the dependency is not in lammps or one of its dependencies are not in lammps
            if(!dependency->existsInLammps(lammpsController) || !dependency->dependenciesValid(lammpsController)) {
                valid = false;
            }
        }
    }

    return valid;
}

void SimulatorControl::update(LAMMPSController *lammpsController)
{
    qDebug() << "Updating control with identifier " << identifier();
    if(!lammpsController->scriptHandler()) {
        return;
    }

    bool exists = existsInLammps(lammpsController);
    qDebug() << identifier() << " exists: " << exists;
    qDebug() << "Enabled: " << m_enabled;
    if(!exists && m_enabled) {
        // We should exist, so let's try to add.
        // Whatever happens, just return. We aren't ready to compute any values yet anyway.
        addToLammps(lammpsController);
        return;
    }

    if(exists && !m_enabled || !dependenciesValid(lammpsController)) {
        // We should not exist, but we do. Now remove from lammps
        lammpsController->scriptHandler()->addCommandToTop(disableCommand());
    }

    if(exists) {
        QString currentCommand = command();
        updateCommand();
        if(currentCommand!=command()) {
            QList<QString> reAddCommands = { disableCommand(), enabledCommand() };
            lammpsController->scriptHandler()->addCommandsToTop(reAddCommands, ScriptCommand::Type::SingleCommand);
        }
    }
}

bool SimulatorControl::enabled() const
{
    return m_enabled;
}

QString SimulatorControl::identifier() const
{
    return m_identifier;
}

QString SimulatorControl::command() const
{
    return m_command;
}

QVariantList SimulatorControl::dependencies() const
{
    return m_dependencies;
}

void SimulatorControl::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;
    m_enabled = enabled;
    emit enabledChanged(enabled);
}

void SimulatorControl::setIdentifier(QString identifier)
{
    if (m_identifier == identifier)
        return;

    m_identifier = identifier;
    emit identifierChanged(identifier);
}

void SimulatorControl::setCommand(QString command)
{
    if (m_command == command)
        return;

    m_command = command;
    emit commandChanged(command);
}

void SimulatorControl::setDependencies(QVariantList dependencies)
{
    if (m_dependencies == dependencies)
        return;

    m_dependencies = dependencies;
    emit dependenciesChanged(dependencies);
}
