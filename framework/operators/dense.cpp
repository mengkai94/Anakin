#include "framework/operators/dense.h"

namespace anakin {

namespace ops {

//#ifdef USE_CUDA
//template<>
//void Dense<NV, AK_FLOAT, Precision::FP32>::operator()(
//    OpContext<NV>& ctx,
//    const std::vector<Tensor4dPtr<NV, AK_FLOAT> >& ins,
//    std::vector<Tensor4dPtr<NV, AK_FLOAT> >& outs) {
//    auto* impl = static_cast<DenseHelper<NV, AK_FLOAT, Precision::FP32>*>(this->_helper);
//    auto& param = static_cast<DenseHelper<NV, AK_FLOAT, Precision::FP32>*>(this->_helper)->_param_dense;
//    impl->_funcs_dense(ins, outs, param, ctx);
//}
//#endif
//
//#ifdef USE_X86_PLACE
//template<>
//void Dense<X86, AK_FLOAT, Precision::FP32>::operator()(
//        OpContext<X86>& ctx,
//        const std::vector<Tensor4dPtr<X86, AK_FLOAT> >& ins,
//        std::vector<Tensor4dPtr<X86, AK_FLOAT> >& outs) {
//    auto* impl = static_cast<DenseHelper<X86, AK_FLOAT, Precision::FP32>*>(this->_helper);
//    auto& param = static_cast<DenseHelper<X86, AK_FLOAT, Precision::FP32>*>(this->_helper)->_param_dense;
//    impl->_funcs_dense(ins, outs, param, ctx);
//}
//#endif

/// TODO ... specialization other type of operator
#define INSTANCE_DENSE(Ttype, Dtype, Ptype) \
template<> \
void Dense<Ttype, Dtype, Ptype>::operator()(OpContext<Ttype>& ctx, \
        const std::vector<Tensor4dPtr<Ttype, Dtype> >& ins, \
        std::vector<Tensor4dPtr<Ttype, Dtype> >& outs) { \
    auto* impl = static_cast<DenseHelper<Ttype, Dtype, Ptype>*>(this->_helper); \
    auto& param = static_cast<DenseHelper<Ttype, Dtype, Ptype>*>(this->_helper)->_param_dense; \
    impl->_funcs_dense(ins, outs, param, ctx); \
}

/// set helper
template<typename Ttype, DataType Dtype, Precision Ptype>
DenseHelper<Ttype, Dtype, Ptype>::~DenseHelper() {
}

template<typename Ttype, DataType Dtype, Precision Ptype>
Status DenseHelper<Ttype, Dtype, Ptype>::InitParam() {
    DLOG(WARNING) << "Parsing Dense op parameter.";
    auto axis = GET_PARAMETER(int, axis);
    auto out_dim = GET_PARAMETER(int, out_dim);
    auto bias_term = GET_PARAMETER(bool, bias_term);

    auto weights = GET_PARAMETER(PBlock<typename DataTypeWarpper<Dtype>::type>, weight_1);

        if (bias_term) {
        auto bias = GET_PARAMETER(PBlock<typename DataTypeWarpper<Dtype>::type>, weight_2);
        saber::FcParam<Tensor4d<Ttype, Dtype>> fc_param(&(weights.d_tensor()), &(bias.d_tensor()), out_dim,
                                            axis);
        _param_dense = fc_param;
    } else {
        Tensor4d<Ttype, Dtype>* bias = nullptr;
        saber::FcParam<Tensor4d<Ttype, Dtype>> fc_param(&(weights.d_tensor()), bias, out_dim, axis);
        _param_dense = fc_param;
    }
    return Status::OK();
}

template<typename Ttype, DataType Dtype, Precision Ptype>
Status DenseHelper<Ttype, Dtype, Ptype>::Init(OpContext<Ttype>& ctx,
        const std::vector<Tensor4dPtr<Ttype, Dtype> >& ins,
        std::vector<Tensor4dPtr<Ttype, Dtype> >& outs) {
    SABER_CHECK(_funcs_dense.init(ins, outs, _param_dense, STATIC, VENDER_IMPL, ctx));
    return Status::OK();
}

template<typename Ttype, DataType Dtype, Precision Ptype>
Status DenseHelper<Ttype, Dtype, Ptype>::InferShape(const std::vector<Tensor4dPtr<Ttype, Dtype> >&
        ins,
        std::vector<Tensor4dPtr<Ttype, Dtype> >& outs) {
    SABER_CHECK(_funcs_dense.compute_output_shape(ins, outs, _param_dense));
    return Status::OK();
}

#ifdef USE_CUDA
INSTANCE_DENSE(NV, AK_FLOAT, Precision::FP32);
template class DenseHelper<NV, AK_FLOAT, Precision::FP32>;
ANAKIN_REGISTER_OP_HELPER(Dense, DenseHelper, NV, AK_FLOAT, Precision::FP32);
template class DenseHelper<NV, AK_FLOAT, Precision::FP16>;
template class DenseHelper<NV, AK_FLOAT, Precision::INT8>;
#endif

#ifdef USE_ARM_PLACE

#ifdef ANAKIN_TYPE_FP32
INSTANCE_DENSE(ARM, AK_FLOAT, Precision::FP32);
template class DenseHelper<ARM, AK_FLOAT, Precision::FP32>;
ANAKIN_REGISTER_OP_HELPER(Dense, DenseHelper, ARM, AK_FLOAT, Precision::FP32);
#endif

#ifdef ANAKIN_TYPE_FP16
template class DenseHelper<ARM, AK_FLOAT, Precision::FP16>;
#endif

#ifdef ANAKIN_TYPE_INT8
template class DenseHelper<ARM, AK_FLOAT, Precision::INT8>;
#endif

#endif

#ifdef USE_X86_PLACE
INSTANCE_DENSE(X86, AK_FLOAT, Precision::FP32);
template class DenseHelper<X86, AK_FLOAT, Precision::FP32>;
ANAKIN_REGISTER_OP_HELPER(Dense, DenseHelper, X86, AK_FLOAT, Precision::FP32);
template class DenseHelper<X86, AK_FLOAT, Precision::FP16>;
template class DenseHelper<X86, AK_FLOAT, Precision::INT8>;
#endif

//! register op
ANAKIN_REGISTER_OP(Dense)
.Doc("Dense operator")
#ifdef USE_CUDA
.__alias__<NV, AK_FLOAT, Precision::FP32>("fullconnect")
.__alias__<NV, AK_FLOAT, Precision::FP32>("fc")
#endif
#ifdef USE_ARM_PLACE
.__alias__<ARM, AK_FLOAT, Precision::FP32>("fullconnect")
.__alias__<ARM, AK_FLOAT, Precision::FP32>("fc")
#endif
#ifdef USE_X86_PLACE
.__alias__<X86, AK_FLOAT, Precision::FP32>("fullconnect")
.__alias__<X86, AK_FLOAT, Precision::FP32>("fc")
#endif
.num_in(1)
.num_out(1)
.Args<int>("axis", " axis to compute ")
.Args<int>("out_dim", " out dim ")
.Args<bool>("bias_term", " whether fc weights have bias");

} /* namespace ops */

} /* namespace anakin */


