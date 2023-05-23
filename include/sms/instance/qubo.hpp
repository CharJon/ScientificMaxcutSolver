#ifndef SMS_QUBO_HPP
#define SMS_QUBO_HPP


#include <string>

/*
 * Represents a QUBO instance.
 * Data is stored in a compact form using variable ids from 0 to d-1.
 */
class QUBO {

public:
    explicit QUBO(int dim);

    QUBO(double *q, int dim);

    ~QUBO();

    void setValue(int i, int j, double value);

    void setValueU(unsigned int i, unsigned int j, double value);

    double getValue(int i, int j) const;

    template<typename T>
    double getSolutionValue(const T *solVector) const {
        double res = 0;
        for (int i = 0; i < dim_; i++) {
            for (int j = 0; j < dim_; j++) {
                res += matrix_[i * dim_ + j] * solVector[i] * solVector[j];
            }
        }
        return res * scalingFactor_ + offset_;
    }

    double getSolutionValueBLAS(const double *solVector) const;

    int getDim() const;

    bool isValid() const;

    void resetToZero();

    void fillAll(double value);

private:
    std::string name_;
    int dim_;
    double *matrix_;
    bool free_;

    double scalingFactor_;
    double offset_;
};


#endif //SMS_QUBO_HPP
