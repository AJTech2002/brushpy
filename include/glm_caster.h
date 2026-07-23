// glm_caster.h
#pragma once
#include "glm/gtc/type_ptr.hpp"
#include <cstring>
#include <glm/glm.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace pybind11 {
namespace detail {

// ---------- shared helpers ----------

// Try to load N floats via the buffer protocol (fast path: pyglm, numpy).
// Returns true and fills `out` on success.
template <size_t N> inline bool load_via_buffer(handle src, float (&out)[N]) {
  if (!PyObject_CheckBuffer(src.ptr()))
    return false;

  Py_buffer buf;
  if (PyObject_GetBuffer(src.ptr(), &buf, PyBUF_FORMAT | PyBUF_ND) != 0) {
    PyErr_Clear();
    return false;
  }

  bool ok = false;
  size_t count = buf.len / (buf.itemsize ? buf.itemsize : sizeof(float));
  if (count == N && buf.itemsize == sizeof(float) && buf.format &&
      (buf.format[0] == 'f')) {
    std::memcpy(out, buf.buf, sizeof(float) * N);
    ok = true;
  }
  PyBuffer_Release(&buf);
  return ok;
}

// Fallback: read N scalars off anything sequence-like (tuple/list/pyglm).
template <size_t N> inline bool load_via_sequence(handle src, float (&out)[N]) {
  if (!py::isinstance<py::sequence>(src))
    return false;
  py::sequence seq = py::reinterpret_borrow<py::sequence>(src);
  if (seq.size() != N)
    return false;
  for (size_t i = 0; i < N; ++i) {
    try {
      out[i] = seq[i].cast<float>();
    } catch (...) {
      return false;
    }
  }
  return true;
}

template <size_t N> inline bool load_floats(handle src, float (&out)[N]) {
  return load_via_buffer<N>(src, out) || load_via_sequence<N>(src, out);
}

// ---------- vec2 / vec3 / vec4 ----------

template <int L, typename T, glm::qualifier Q>
struct type_caster<glm::vec<L, T, Q>> {
  using value_type = glm::vec<L, T, Q>;
  PYBIND11_TYPE_CASTER(value_type, const_name("glm.vec") + const_name<L>());

  bool load(handle src, bool) {
    float tmp[L];
    if (!load_floats<L>(src, tmp))
      return false;
    for (int i = 0; i < L; ++i)
      value[i] = static_cast<T>(tmp[i]);
    return true;
  }

  static handle cast(const value_type &src, return_value_policy, handle) {
    static py::object cls = py::module_::import("glm").attr(L == 2   ? "vec2"
                                                            : L == 3 ? "vec3"
                                                                     : "vec4");
    if (L == 2)
      return cls(src[0], src[1]).release();
    if (L == 3)
      return cls(src[0], src[1], src[2]).release();
    return cls(src[0], src[1], src[2], src[3]).release();
  }
};

// ---------- mat4 ----------

template <typename T, glm::qualifier Q>
struct type_caster<glm::mat<4, 4, T, Q>> {
  using value_type = glm::mat<4, 4, T, Q>;
  PYBIND11_TYPE_CASTER(value_type, const_name("glm.mat4x4"));

  bool load(handle src, bool) {
    float tmp[16];
    // Fast path: buffer protocol gives the raw 16 floats directly,
    // column-major, matching glm's in-memory layout exactly.
    if (load_via_buffer<16>(src, tmp)) {
      std::memcpy(glm::value_ptr(value), tmp, sizeof(tmp));
      return true;
    }
    // Fallback: nested sequence of 4 columns (each a 4-length sequence),
    // e.g. a plain list of lists, or matrix.to_tuple().
    if (py::isinstance<py::sequence>(src)) {
      py::sequence outer = py::reinterpret_borrow<py::sequence>(src);
      if (outer.size() == 4) {
        for (int c = 0; c < 4; ++c) {
          float col[4];
          if (!load_via_sequence<4>(outer[c], col))
            return false;
          for (int r = 0; r < 4; ++r)
            value[c][r] = static_cast<T>(col[r]);
        }
        return true;
      }
      // Flat 16-element sequence fallback (no buffer protocol support).
      float flat[16];
      if (load_via_sequence<16>(src, flat)) {
        std::memcpy(glm::value_ptr(value), flat, sizeof(flat));
        return true;
      }
    }
    return false;
  }

  static handle cast(const value_type &src, return_value_policy, handle) {
    static py::object mat4_cls = py::module_::import("glm").attr("mat4");
    const float *p = glm::value_ptr(src);
    return mat4_cls(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9],
                    p[10], p[11], p[12], p[13], p[14], p[15])
        .release();
  }
};

} // namespace detail
} // namespace pybind11