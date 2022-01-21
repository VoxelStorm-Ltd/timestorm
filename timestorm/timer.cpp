#include "timer.h"

namespace timestorm {

// explicit instantiations
template class timer<float, std::ostream>;
template class timer<double, std::ostream>;
template class timer<int, std::ostream>;
template class timer<unsigned int, std::ostream>;

}
