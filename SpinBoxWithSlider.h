/*
 * SpinBoxWithSlider.h
 *
 *  Created on: Nov 14, 2014
 *      Author: tobias
 */

#ifndef SPINBOXWITHSLIDER_H_
#define SPINBOXWITHSLIDER_H_

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QString>


class SteppedSpinBox: public QSpinBox {
    Q_OBJECT
  public:
    SteppedSpinBox(QWidget *p = nullptr);

  protected:
    virtual int valueFromText(const QString &text) const override;
};


class SpinBoxWithSlider: public QWidget {
    Q_OBJECT
  public:
    /**
     * Integer Spinbox + Slider.
     * representable values: {min, min + step_size, min + 2 * step_size, ..., max - step_size, max}
     *     Default (step_size = 1): {min, min + 1, min + 2, ..., max - 1, max}
     *
     * @param min lower bound
     * @param max upper bound, max >= min && max == min + n * step_size; \f$n \in \mathbb{N}\f$
     * @param start_value min <= start_value <= max && start_value == min + n * step_size; \f$n \in \mathbb{N}\f$
     * @param step_size >= 1
     */
    explicit SpinBoxWithSlider(QWidget *parent, const QString &name, int min,
                               int max, int start_value, int step_size = 1);
    explicit SpinBoxWithSlider(QWidget *parent, QFormLayout *layout,
                               const QString &name, int min, int max, int start_value, int step_size = 1);

    SpinBoxWithSlider(const SpinBoxWithSlider &) = delete;
    SpinBoxWithSlider &operator=(const SpinBoxWithSlider &) = delete;

    int value() const {
        return m_spinbox.value();
    }

    void setValue(const int value) {
        m_spinbox.setValue(value);
    }

    virtual ~SpinBoxWithSlider() override = default;

  private slots:
    void SpinBoxValueChanged(int val);
    void SliderValueChanged(int value);

  signals:
    void valueChanged(int i);

  private:
    int            m_step_size;
    QHBoxLayout    m_layout;
    QLabel         m_name;
    QSlider        m_slider;
    SteppedSpinBox m_spinbox;
};


class DoubleSpinBoxWithSlider: public QWidget {
    Q_OBJECT
  public:
    /**
     * double Spinbox + Slider.
     *
     * @param precision : decimal places
     */
    explicit DoubleSpinBoxWithSlider(QWidget *parent, const QString &name,
                                     double min, double max, double start_value, int precision = 1);
    explicit DoubleSpinBoxWithSlider(QWidget *parent, QFormLayout *layout,
                                     const QString &name, double min, double max, double start_value,
                                     int precision = 1);

    DoubleSpinBoxWithSlider(const DoubleSpinBoxWithSlider &) = delete;
    DoubleSpinBoxWithSlider &operator=(const DoubleSpinBoxWithSlider &) = delete;

    double value() const {
        return m_spinbox.value();
    }

    virtual ~DoubleSpinBoxWithSlider() override = default;

  private slots:
    void SpinBoxValueChanged(double val);
    void SliderValueChanged(int value);

  signals:
    void valueChanged(double i);

  private:
    int            m_factor;
    QHBoxLayout    m_layout;
    QLabel         m_name;
    QSlider        m_slider;
    QDoubleSpinBox m_spinbox;
};


#endif /* SPINBOXWITHSLIDER_H_ */
