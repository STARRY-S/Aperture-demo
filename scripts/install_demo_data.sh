#!/bin/bash

set -euo pipefail

echo "MESON_INSTALL_DESTDIR_PREFIX: ${MESON_INSTALL_DESTDIR_PREFIX}"
echo "MESON_SOURCE_ROOT: ${MESON_SOURCE_ROOT}"

DEMO_DATA_DST="${MESON_INSTALL_DESTDIR_PREFIX}/share/aperture_demo/"
DEMO_DATA_SRC="${MESON_SOURCE_ROOT}/dists/"

mkdir -p "${DEMO_DATA_DST}"
cp -r "${DEMO_DATA_SRC}/fonts" "${DEMO_DATA_DST}/"
cp -r "${DEMO_DATA_SRC}/glsl"  "${DEMO_DATA_DST}/"
cp -r "${DEMO_DATA_SRC}/mc"    "${DEMO_DATA_DST}/"
cp -r "${DEMO_DATA_SRC}/sound" "${DEMO_DATA_DST}/"
