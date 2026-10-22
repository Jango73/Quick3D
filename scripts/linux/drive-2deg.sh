#!/bin/bash
# Drive Man1 two latitude degrees north in headed Quick3DTest, chase camera,
# through the remote control HTTP server. Repro for the total terrain
# disappearance at high speed: stop is explicit (latitude read on /vehicle),
# never a judgement call, with a safety timeout that always releases keys.
# Usage: ./scripts/linux/drive-2deg.sh [--build debug|release] [--speed N] [--stop LAT] [--timeout S]
# Environment assumptions (overridable, defaults in brackets):
# - Q3D_REMOTE_PORT: HTTP remote control port, passed to the app [8181].
# - DISPLAY: X display used for the headed run [:1].
# - fuser: used to free the remote port before launch.
# - python3: used to parse the string-only JSON produced by CXMLNode
#   (scalars come back as strings, e.g. "latitude":"21.5", "ok":"true").
set -u

SCRIPT_FOLDER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT_FOLDER="$(cd "${SCRIPT_FOLDER}/../.." && pwd)"

BUILD_TYPE="release"
SPEED=50
STOP_LAT=22.1
TIMEOUT=900
while [[ $# -gt 0 ]]; do
  case "$1" in
    --build) BUILD_TYPE="$2"; shift 2;;
    --speed) SPEED="$2"; shift 2;;
    --stop) STOP_LAT="$2"; shift 2;;
    --timeout) TIMEOUT="$2"; shift 2;;
    *) echo "Usage: $0 [--build debug|release] [--speed N] [--stop LAT] [--timeout S]"; exit 1;;
  esac
done

BUILD_FOLDER="${PROJECT_ROOT_FOLDER}/build-${BUILD_TYPE}"
BINARY="${BUILD_FOLDER}/quick3dtest/Quick3DTest"
SCENE="${BUILD_FOLDER}/quick3dtest/Quick3DDrive.xml"
PORT="${Q3D_REMOTE_PORT:-8181}"
HEADED_DISPLAY="${DISPLAY:-:1}"
VEHICLE="Man1"
PROGRESS="${PROJECT_ROOT_FOLDER}/tmp/drive-progress.log"
REPORT="${PROJECT_ROOT_FOLDER}/tmp/drive-2deg-report.txt"

command -v curl >/dev/null || { echo "missing curl"; exit 1; }
command -v python3 >/dev/null || { echo "missing python3"; exit 1; }
command -v fuser >/dev/null || { echo "missing fuser"; exit 1; }
[[ -x "${BINARY}" ]] || { echo "missing binary: ${BINARY}"; exit 1; }
[[ -f "${SCENE}" ]] || { echo "missing scene: ${SCENE} (rebuild to mirror bin/)"; exit 1; }
mkdir -p "${PROJECT_ROOT_FOLDER}/tmp"

api() {
  curl -s -m 10 "http://127.0.0.1:${PORT}$1"
}

echo "[drive] starting Quick3DTest --scene Quick3DDrive.xml --check-render (port ${PORT})"
fuser -k ${PORT}/tcp 2>/dev/null
sleep 1
rm -f "${PROGRESS}"
cd "${BUILD_FOLDER}/quick3dtest" || exit 1
setsid env QUICK3D_ENDLESS=1 Q3D_REMOTE_PORT=${PORT} DISPLAY=${HEADED_DISPLAY} \
  "${BINARY}" --scene Quick3DDrive.xml --check-render >/dev/null 2>&1 < /dev/null &
APP_PID=$!
trap 'kill ${APP_PID} 2>/dev/null' EXIT
cd "${PROJECT_ROOT_FOLDER}" || exit 1

echo "[drive] waiting for HTTP" | tee "${PROGRESS}"
for i in $(seq 1 60); do
  api "/" >/dev/null 2>&1 && break
  sleep 2
done

START=$(api "/vehicle?name=${VEHICLE}" || true)
echo "${START}" | grep -q '"ok":"true"' || { echo "scene not loaded: ${START}"; exit 1; }
echo "[drive] start ${START}" | tee -a "${PROGRESS}"
echo "[drive] select $(api "/select?name=${VEHICLE}")" | tee -a "${PROGRESS}"
echo "[drive] view $(api "/view?name=Man1.Head")" | tee -a "${PROGRESS}"
echo "[drive] speed $(api "/speed?value=${SPEED}")" | tee -a "${PROGRESS}"
api "/key?code=Z&action=press" >/dev/null
echo "[drive] Z held, stop latitude ${STOP_LAT}, timeout ${TIMEOUT}s" | tee -a "${PROGRESS}"

ELAPSED=0
while [[ ${ELAPSED} -lt ${TIMEOUT} ]]; do
  sleep 10
  ELAPSED=$((ELAPSED + 10))
  STATUS=$(api "/vehicle?name=${VEHICLE}" || true)
  LAT=$(echo "${STATUS}" | python3 -c 'import sys,json; print(str(json.load(sys.stdin).get("latitude", "-999")))' 2>/dev/null || echo "-999")
  ALIVE="0"
  kill -0 ${APP_PID} 2>/dev/null && ALIVE="1"
  echo "[drive] t=${ELAPSED}s alive=${ALIVE} lat=${LAT}" | tee -a "${PROGRESS}"
  if [[ "${ALIVE}" == "0" ]]; then
    echo "RESULT=APP_DIED t=${ELAPSED}s last_lat=${LAT}" | tee -a "${PROGRESS}"
    exit 2
  fi
  REACHED=$(echo "${LAT} ${STOP_LAT}" | python3 -c 'import sys; v,s = map(float, sys.stdin.read().split()); print(1 if v >= s else 0)' 2>/dev/null || echo "0")
  if [[ "${REACHED}" == "1" ]]; then
    api "/key?code=Z&action=release" >/dev/null
    sleep 2
    kill ${APP_PID} 2>/dev/null
    echo "RESULT=REACHED t=${ELAPSED}s lat=${LAT}" | tee -a "${PROGRESS}"
    break
  fi
done

if [[ ${ELAPSED} -ge ${TIMEOUT} ]]; then
  api "/key?code=Z&action=release" >/dev/null
  kill ${APP_PID} 2>/dev/null
  echo "RESULT=TIMEOUT t=${ELAPSED}s last_lat=${LAT}" | tee -a "${PROGRESS}"
  exit 1
fi

trap - EXIT
LOG="${BUILD_FOLDER}/quick3dtest/Logs/Quick3DTest.log"
{
  echo "RESULT=REACHED t=${ELAPSED}s lat=${LAT}"
  echo -n "empty_pairs="; grep -ac "LayerPartitionEmptyPair" "${LOG}" 2>/dev/null || echo 0
  echo -n "draw_starved="; grep -ac "LayerDrawStarved" "${LOG}" 2>/dev/null || echo 0
} | tee "${REPORT}"
