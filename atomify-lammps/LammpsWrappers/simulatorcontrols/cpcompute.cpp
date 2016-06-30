#include "cpcompute.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include "cpdata.h"
#include "../system.h"
#include <QDebug>

CPCompute::CPCompute(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{

}

CPCompute::~CPCompute() { }

CP1DData *CPCompute::ensureExists(QString key, bool enabledByDefault) {
    if(!m_data1DRaw.contains(key)) {
        CP1DData *data = new CP1DData(this);
        data->setEnabled(enabledByDefault);
        m_data1DRaw.insert(key, data);
        m_data1D.insert(key, QVariant::fromValue<CP1DData*>(data));
    }
    return m_data1DRaw[key];
}

bool CPCompute::copyData(ComputeTemp *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double value = compute->compute_scalar();
    setHasScalarData(true);
    setScalarValue(value);
    CP1DData *data = ensureExists(QString("Temperature"), true);
    setXLabel("Time");
    setYLabel("Temperature");
    data->add(lammpsController->system()->simulationTime(), value);
    return true;
}

bool CPCompute::copyData(ComputePE *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double value = compute->compute_scalar();
    setHasScalarData(true);
    setScalarValue(value);
    CP1DData *data = ensureExists(QString("Potential energy"), true);
    setXLabel("Time");
    setYLabel("Potential Energy");
    data->add(lammpsController->system()->simulationTime(), value);
    return true;
}

bool CPCompute::copyData(ComputeKE *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    double value = compute->compute_scalar();
    setHasScalarData(true);
    setScalarValue(value);
    CP1DData *data = ensureExists(QString("Kinetic energy"), true);
    setXLabel("Time");
    setYLabel("Kinetic Energy");
    data->add(lammpsController->system()->simulationTime(), value);
    return true;
}

bool CPCompute::copyData(ComputePressure *compute, LAMMPSController *lammpsController) {
    if(!compute) return false;
    // First compute scalar pressure
    double value = compute->compute_scalar();
    setHasScalarData(true);
    setScalarValue(value);
    CP1DData *data = ensureExists(QString("Pressure"), true);
    setXLabel("Time");
    setYLabel("Pressure");
    data->add(lammpsController->system()->simulationTime(), value);

    // Then compute stress tensor
    compute->compute_vector();
    // xx, yy, zz, xy, xz, yz
    QStringList components = {"xx", "yy", "zz", "xy", "xz", "yz"};

    int numVectorValues = 6;
    for(int i=1; i<=numVectorValues; i++) {
        QString key = components[i-1];
        CP1DData *data = ensureExists(key, false);
        double value = compute->vector[i-1];
        data->add(lammpsController->system()->simulationTime(), value);
    }
    return true;
}

void CPCompute::copyData(LAMMPSController *lammpsController)
{
    if(lammpsController->system()->timesteps() % m_frequency != 0) return;
    Compute *lmp_compute = lammpsController->findComputeByIdentifier(identifier());
    if(lmp_compute == nullptr) return;

    if(copyData(dynamic_cast<ComputePressure*>(lmp_compute), lammpsController)) return;
    if(copyData(dynamic_cast<ComputeTemp*>(lmp_compute), lammpsController)) return;
    if(copyData(dynamic_cast<ComputeKE*>(lmp_compute), lammpsController)) return;
    if(copyData(dynamic_cast<ComputePE*>(lmp_compute), lammpsController)) return;

    if(lmp_compute->scalar_flag == 1) {
        double value = lmp_compute->compute_scalar();
        setHasScalarData(true);
        setScalarValue(value);
        CP1DData *data = ensureExists("scalar", true);
        data->add(lammpsController->system()->simulationTime(), value);
    }
    if(lmp_compute->vector_flag == 1) {
        lmp_compute->compute_vector();
        int numVectorValues = lmp_compute->size_vector;
        for(int i=1; i<=numVectorValues; i++) {
            QString key = QString("%1").arg(i);
            CP1DData *data = ensureExists(key, true);
            double value = lmp_compute->vector[i-1];
            data->add(lammpsController->system()->simulationTime(), value);
        }
    }
    if(lmp_compute->array_flag == 1) {

    }
}

void CPCompute::updateCommand()
{
    // For standard computes, command doesn't change
}

QList<QString> CPCompute::enabledCommands()
{
    return { fullCommand() };
}

QList<QString> CPCompute::disableCommands()
{
    return {QString("uncompute %1").arg(identifier())};
}

bool CPCompute::existsInLammps(LAMMPSController *lammpsController)
{
    Compute *compute = lammpsController->findComputeByIdentifier(identifier());
    return compute!=nullptr;
}

bool CPCompute::isVector() const
{
    return m_isVector;
}

QString CPCompute::group() const
{
    return m_group;
}

int CPCompute::frequency() const
{
    return m_frequency;
}

bool CPCompute::hasScalarData() const
{
    return m_hasScalarData;
}

float CPCompute::scalarValue() const
{
    return m_scalarValue;
}

int CPCompute::num1DData() const
{
    return m_num1DData;
}

QVariantMap CPCompute::data1D() const
{
    return m_data1D;
}

QString CPCompute::xLabel() const
{
    return m_xLabel;
}

QString CPCompute::yLabel() const
{
    return m_yLabel;
}

void CPCompute::setIsVector(bool isVector)
{
    if (m_isVector == isVector)
        return;

    m_isVector = isVector;
    emit isVectorChanged(isVector);
}

void CPCompute::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    emit groupChanged(group);
}

void CPCompute::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(frequency);
}

void CPCompute::setHasScalarData(bool hasScalarData)
{
    if (m_hasScalarData == hasScalarData)
        return;

    m_hasScalarData = hasScalarData;
    emit hasScalarDataChanged(hasScalarData);
}

void CPCompute::setScalarValue(float scalarValue)
{
    if (m_scalarValue == scalarValue)
        return;

    m_scalarValue = scalarValue;
    emit scalarValueChanged(scalarValue);
}

void CPCompute::setNum1DData(int num1DData)
{
    if (m_num1DData == num1DData)
        return;

    m_num1DData = num1DData;
    emit num1DDataChanged(num1DData);
}

void CPCompute::setData1D(QVariantMap data1D)
{
    if (m_data1D == data1D)
        return;

    m_data1D = data1D;
    emit data1DChanged(data1D);
}

void CPCompute::setXLabel(QString xLabel)
{
    if (m_xLabel == xLabel)
        return;

    m_xLabel = xLabel;
    emit xLabelChanged(xLabel);
}

void CPCompute::setYLabel(QString yLabel)
{
    if (m_yLabel == yLabel)
        return;

    m_yLabel = yLabel;
    emit yLabelChanged(yLabel);
}

QList<QString> CPCompute::resetCommands()
{
    return { QString("uncompute %1").arg(identifier()), fullCommand() };
}

QString CPCompute::createCommandPrefix()
{
    return QString("compute %1 %2 ").arg(identifier()).arg(group());
}
