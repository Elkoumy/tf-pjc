licenses(["notice"])  # Apache 2.0

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = 'python/_pjc_ops.so',
    srcs = [
        "cc/ops.cc",
        "cc/kernels.cc",
    ],
    linkshared = True,
    deps = [
        # TODO(Morten) the following seems like a mistake
        "@com_google_absl//absl/container:inlined_vector",

        "@local_config_tf//:libtensorflow_framework",
        "@local_config_tf//:tf_header_lib",

        "@com_github_google_private_join_and_compute//:lib",
    ],
    copts = ["-pthread", "-std=c++11", "-D_GLIBCXX_USE_CXX11_ABI=0", "-fPIC"],
)

py_library(
    name = "pjc_ops_py",
    srcs = ([
        "python/ops.py",
        "python/protocol.py",
    ]),
    data = [
        ":python/_pjc_ops.so"
    ],
    srcs_version = "PY3",
)

py_test(
    name = "pjc_ops_py_test",
    srcs = [
        "python/ops_test.py",
    ],
    main = "python/ops_test.py",
    deps = [
        ":pjc_ops_py",
    ],
    srcs_version = "PY3",
)

py_test(
    name = "pjc_protocol_py_test",
    srcs = [
        "python/protocol_test.py",
    ],
    main = "python/protocol_test.py",
    deps = [
        ":pjc_ops_py",
    ],
    srcs_version = "PY3",
)
