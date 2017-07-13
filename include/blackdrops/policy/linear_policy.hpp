//| Copyright Inria July 2017
//| This project has received funding from the European Research Council (ERC) under
//| the European Union's Horizon 2020 research and innovation programme (grant
//| agreement No 637972) - see http://www.resibots.eu
//|
//| Contributor(s):
//|   - Konstantinos Chatzilygeroudis (konstantinos.chatzilygeroudis@inria.fr)
//|   - Roberto Rama (bertoski@gmail.com)
//|
//| This software is a computer library whose purpose is to optimize continuous,
//| black-box functions. It mainly implements Gaussian processes and Bayesian
//| optimization.
//| Main repository: http://github.com/resibots/blackdrops
//|
//| This software is governed by the CeCILL-C license under French law and
//| abiding by the rules of distribution of free software.  You can  use,
//| modify and/ or redistribute the software under the terms of the CeCILL-C
//| license as circulated by CEA, CNRS and INRIA at the following URL
//| "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and  rights to copy,
//| modify and redistribute granted by the license, users are provided only
//| with a limited warranty  and the software's author,  the holder of the
//| economic rights,  and the successive licensors  have only  limited
//| liability.
//|
//| In this respect, the user's attention is drawn to the risks associated
//| with loading,  using,  modifying and/or developing or reproducing the
//| software by the user in light of its specific status of free software,
//| that may mean  that it is complicated to manipulate,  and  that  also
//| therefore means  that it is reserved for developers  and  experienced
//| professionals having in-depth computer knowledge. Users are therefore
//| encouraged to load and test the software's suitability as regards their
//| requirements in conditions enabling the security of their systems and/or
//| data to be ensured and,  more generally, to use and operate it in the
//| same conditions as regards security.
//|
//| The fact that you are presently reading this means that you have had
//| knowledge of the CeCILL-C license and that you accept its terms.
//|
#ifndef BLACKDROPS_POLICY_LINEAR_POLICY_HPP
#define BLACKDROPS_POLICY_LINEAR_POLICY_HPP

#include <limbo/tools/random_generator.hpp>

namespace blackdrops {
    namespace policy {
        template <typename Params>
        struct LinearPolicy {

            LinearPolicy() { _random = false; }

            Eigen::VectorXd next(const Eigen::VectorXd& state) const
            {
                if (_random || _params.size() == 0) {
                    Eigen::VectorXd act = (limbo::tools::random_vector(Params::linear_policy::action_dim()).array() * 2 - 1.0);
                    for (int i = 0; i < act.size(); i++) {
                        act(i) = act(i) * Params::linear_policy::max_u(i);
                    }
                    return act;
                }

                Eigen::VectorXd act = _alpha * state + _constant;

                for (int i = 0; i < act.size(); i++) {
                    act(i) = Params::linear_policy::max_u(i) * (9 * std::sin(act(i)) / 8.0 + std::sin(3 * act(i)) / 8.0);
                }

                return act;
            }

            void set_random_policy()
            {
                _random = true;
            }

            bool random() const
            {
                return _random;
            }

            void set_params(const Eigen::VectorXd& params)
            {
                size_t M = Params::linear_policy::action_dim();
                size_t N = Params::linear_policy::state_dim();

                _params = params;
                _alpha = Eigen::MatrixXd::Zero(M, N);
                _constant = Eigen::VectorXd::Zero(M);
                for (size_t i = 0; i < M; i++) {
                    for (size_t j = 0; j < N; j++) {
                        _alpha(i, j) = params(i * M + j);
                    }
                }

                for (size_t i = N * M; i < (N + 1) * M; i++)
                    _constant(i - N * M) = params(i);

                _random = false;
            }

            Eigen::VectorXd params() const
            {
                if (_random || _params.size() == 0)
                    return limbo::tools::random_vector((Params::linear_policy::state_dim() + 1) * Params::linear_policy::action_dim());
                return _params;
            }

            Eigen::MatrixXd _alpha;
            Eigen::VectorXd _constant, _params;
            bool _random;
        };
    }
}
#endif
