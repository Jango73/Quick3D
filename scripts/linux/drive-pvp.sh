#!/bin/bash
# Drive Visitor2 (PVP) in headed Quick3DTest through the remote control HTTP server.
# No application change: keys go through the UI, selection through the combo box.
# Usage: ./scripts/linux/drive-pvp.sh [--build debug|release]
set -u

SCRIPT_FOLDER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT_FOLDER="$(cd "${SCRIPT_FOLDER}/../.." && pwd)"

BUILD_TYPE="release"
while [[ $# -gt 0 ]]; do
  case "$1" in
    --build) BUILD_TYPE="$2"; shift 2;;
    *) echo "Usage: $0 [--build debug|release]"; exit 1;;
  esac
done

BUILD_FOLDER="${PROJECT_ROOT_FOLDER}/build-${BUILD_TYPE}"
BINARY="${BUILD_FOLDER}/quick3dtest/Quick3DTest"
PORT=8181
VEHICLE="Visitor2"
SAMPLES="${PROJECT_ROOT_FOLDER}/tmp/drive_pvp_samples.jsonl"
REPORT="${PROJECT_ROOT_FOLDER}/tmp/drive_pvp_report.txt"

command -v curl >/dev/null || { echo "curl manquant"; exit 1; }
command -v python3 >/dev/null || { echo "python3 manquant"; exit 1; }
command -v fuser >/dev/null || { echo "fuser manquant"; exit 1; }
[[ -x "${BINARY}" ]] || { echo "binaire absent: ${BINARY}"; exit 1; }
mkdir -p "${PROJECT_ROOT_FOLDER}/tmp"

api() {
  curl -s -m 10 "http://127.0.0.1:${PORT}$1"
}

echo "[drive] lancement Quick3DTest (port ${PORT})"
fuser -k ${PORT}/tcp 2>/dev/null
sleep 1
Q3D_REMOTE_PORT=${PORT} "${BINARY}" >/dev/null 2>&1 &
APP_PID=$!
trap 'kill ${APP_PID} 2>/dev/null' EXIT

echo "[drive] attente chargement scene"
for i in $(seq 1 60); do
  STATUS=$(api "/vehicle?name=${VEHICLE}" || true)
  echo "${STATUS}" | grep -q '"ok":true' && break
  sleep 2
done
echo "${STATUS}" | grep -q '"ok":true' || { echo "scene non chargee: ${STATUS}"; exit 1; }

echo "[drive] selection ${VEHICLE} via UI + vue driver (avant le sol, pour voir le tassement)"
api "/select?name=${VEHICLE}"
api "/view?name=Visitor2.Driver" || { echo "vue refusee"; exit 1; }
STATUS=$(api "/vehicle?name=${VEHICLE}")
echo "${STATUS}" | grep -q '"controlled":true' || { echo "selection refusee: ${STATUS}"; exit 1; }

echo "[drive] attente sol sous les roues (physics + 4 contacts)"
START_WAIT=$(date +%s)
for i in $(seq 1 150); do
  STATUS=$(api "/vehicle?name=${VEHICLE}" || true)
  echo "${STATUS}" | grep -q '"physics":true' || { sleep 2; continue; }
  CONTACTS=$(echo "${STATUS}" | python3 -c 'import json,sys; print(json.load(sys.stdin).get("contacts", -1))')
  echo "[drive] $(( $(date +%s) - START_WAIT ))s contacts=${CONTACTS}"
  [[ "${CONTACTS}" == "4" ]] && break
  sleep 2
done
[[ "${CONTACTS}" == "4" ]] || { echo "pas de sol sous les 4 roues: ${STATUS}"; exit 1; }

echo "[drive] H (desserrer frein)"
api "/key?code=H&action=click"
sleep 1
STATUS=$(api "/vehicle?name=${VEHICLE}")
echo "${STATUS}" | grep -q '"handBrake":false' || { echo "frein toujours serre: ${STATUS}"; exit 1; }

: > "${SAMPLES}"
echo "[drive] Z maintenu 15 s, echantillonnage 1 Hz"
api "/key?code=Z&action=press"
for i in $(seq 1 15); do
  api "/vehicle?name=${VEHICLE}" >> "${SAMPLES}"
  echo "" >> "${SAMPLES}"
  sleep 1
done
echo "[drive] Z+W (braquage gauche) 12 s"
api "/key?code=W&action=press"
for i in $(seq 1 12); do
  api "/vehicle?name=${VEHICLE}" >> "${SAMPLES}"
  echo "" >> "${SAMPLES}"
  sleep 1
done
api "/key?code=W&action=release"
api "/key?code=Z&action=release"
echo "[drive] 3 echantillons post-arret"
for i in $(seq 1 3); do
  sleep 1
  api "/vehicle?name=${VEHICLE}" >> "${SAMPLES}"
  echo "" >> "${SAMPLES}"
done

python3 - "${SAMPLES}" "${REPORT}" <<'EOF'
import json, math, sys
samples_path, report_path = sys.argv[1], sys.argv[2]
samples = []
with open(samples_path) as f:
    for line in f:
        line = line.strip()
        if line:
            samples.append(json.loads(line))
drive = samples[:15]
turn = samples[15:27]
lat0 = math.radians(drive[0]["latitude"])
def meters(s):
    x = (math.radians(s["longitude"]) - math.radians(drive[0]["longitude"])) * 6371000.0 * math.cos(lat0)
    y = (math.radians(s["latitude"]) - math.radians(drive[0]["latitude"])) * 6371000.0
    return x, y
starts = meters(drive[0])
ends = meters(drive[-1])
distance = math.hypot(ends[0] - starts[0], ends[1] - starts[1])
yaws = [math.degrees(s["rotationY"]) for s in drive]
rolls = [math.degrees(s["rotationZ"]) for s in drive]
alts = [s["altitude"] for s in drive]
drive_contacts = [s["contacts"] for s in drive]
turn_contacts = [s["contacts"] for s in turn]
def contact_text(values, label):
    full = sum(1 for c in values if c == 4)
    return f"{label}_roues_min={min(values)} {label}_roues_moy={sum(values)/len(values):.1f} {label}_4roues={full}/{len(values)}"
turn_xy = [meters(s) for s in turn]
tx = [p[0] for p in turn_xy]
ty = [p[1] for p in turn_xy]
n = len(tx)
mx, my = sum(tx) / n, sum(ty) / n
sxx = sum((x - mx) ** 2 for x in tx)
sxy = sum((x - mx) * (y - my) for x, y in zip(tx, ty))
syy = sum((y - my) ** 2 for y in ty)
sxz = sum((x - mx) * ((x - mx) ** 2 + (y - my) ** 2) for x, y in zip(tx, ty))
syz = sum((y - my) * ((x - mx) ** 2 + (y - my) ** 2) for x, y in zip(tx, ty))
den = 2.0 * (sxx * syy - sxy * sxy)
if abs(den) < 1e-9:
    radius_text = "rayon=infini (tout droit)"
else:
    uc = (syy * sxz - sxy * syz) / den
    vc = (sxx * syz - sxy * sxz) / den
    radius = math.sqrt(uc * uc + vc * vc + (sxx + syy) / n)
    yaw_sweep = math.degrees(turn[-1]["rotationY"]) - math.degrees(turn[0]["rotationY"])
    radius_text = f"rayon={radius:.1f}m cap={yaw_sweep:.0f}deg"
report = (
    f"echan={len(drive)} "
    f"distance_15s={distance:.2f}m "
    f"yaw_min={min(yaws):.1f}deg yaw_max={max(yaws):.1f}deg yaw_amp={max(yaws)-min(yaws):.1f}deg "
    f"roll_amp={max(rolls)-min(rolls):.1f}deg "
    f"alt_min={min(alts):.2f}m alt_max={max(alts):.2f}m "
    f"virage={radius_text} "
    f"{contact_text(drive_contacts, 'ligne')} "
    f"{contact_text(turn_contacts, 'virage')}"
)
print(report)
with open(report_path, "w") as f:
    f.write(report + "\n")
EOF

echo "[drive] rapport: ${REPORT}"
kill ${APP_PID} 2>/dev/null
trap - EXIT
