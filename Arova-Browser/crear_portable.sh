#!/bin/bash
# Crea la version portable de Arova con todo incluido (binarios, librerias,
# plugins, traducciones, recursos de QtWebEngine, fuentes, certificados y config).
# Uso: ./crear_portable.sh [carpeta_de_salida]

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# --- Rutas de origen (editalas si tu sistema usa otras) ---
AROVA_BIN="${AROVA_BIN:-$SCRIPT_DIR/build/src/arova}"
QT_PLUGINS_DIR="${QT_PLUGINS_DIR:-/usr/lib/x86_64-linux-gnu/qt6/plugins}"
QT_QML_DIR="${QT_QML_DIR:-/usr/lib/x86_64-linux-gnu/qt6/qml}"
QT_TRANSLATIONS_DIR="${QT_TRANSLATIONS_DIR:-/usr/share/qt6/translations}"
QT_RESOURCES_DIR="${QT_RESOURCES_DIR:-/usr/share/qt6/resources}"
WEBENGINE_PROCESS="${WEBENGINE_PROCESS:-/usr/lib/qt6/libexec/QtWebEngineProcess}"
APP_LOCALE_DIR="${APP_LOCALE_DIR:-$SCRIPT_DIR/build/src/locale}"
FONTS_DIRS="${FONTS_DIRS:-/usr/share/fonts /usr/local/share/fonts}"
CERT_SRC="${CERT_SRC:-/etc/ssl/certs}"
COPY_FONTS="${COPY_FONTS:-1}"
# Las librerias GL/Mesa/Vulkan/DRI se dejan al sistema: los drivers de GPU se
# cargan en runtime con rutas compiladas y deben coincidir con la GPU/kernel.
GL_LIBS='libGL.so.1 libGLX.so.0 libGLdispatch.so.0 libEGL.so.1 libOpenGL.so.0 libGLESv2.so.2 libGLESv1_CM.so.1 libGLX_mesa.so.0 libEGL_mesa.so.0 libgbm.so.1 libdrm.so.2 libdrm_amdgpu.so.1 libdrm_intel.so.1 libdrm_nouveau.so.2 libdrm_radeon.so.1 libgallium*.so* libLLVM*.so* libvulkan.so.1 libvk_swiftshader.so libxcb-present.so.0 libxshmfence.so.1 libXxf86vm.so.1 libedit.so.2 libtinfo.so.6 libpciaccess.so.0 libpci.so.3 libsensors.so.5'

is_gl_lib() {
    local base
    base="$(basename "$1")"
    local p
    for p in $GL_LIBS; do
        case "$base" in
            $p) return 0 ;;
        esac
    done
    return 1
}

OUT="${1:-$SCRIPT_DIR/Arova-portable}"

error() { echo "ERROR: $1" >&2; exit 1; }
[ -f "$AROVA_BIN" ] || error "No se encontro el binario: $AROVA_BIN"
[ -d "$QT_PLUGINS_DIR" ] || error "No se encontro el directorio de plugins: $QT_PLUGINS_DIR"
[ -d "$QT_QML_DIR" ] || error "No se encontro el directorio QML: $QT_QML_DIR"
[ -d "$QT_TRANSLATIONS_DIR" ] || error "No se encontro el directorio de traducciones: $QT_TRANSLATIONS_DIR"
[ -d "$QT_RESOURCES_DIR" ] || error "No se encontro el directorio de recursos: $QT_RESOURCES_DIR"
[ -f "$WEBENGINE_PROCESS" ] || error "No se encontro QtWebEngineProcess: $WEBENGINE_PROCESS"

echo "Creando el portable en: $OUT"
if [ -e "$OUT" ]; then
    echo "  (se eliminara la carpeta existente)"
    rm -rf "$OUT"
fi

mkdir -p "$OUT/bin" "$OUT/lib" "$OUT/plugins" "$OUT/qml" "$OUT/translations" \
    "$OUT/libexec" "$OUT/locale" "$OUT/data/resources" "$OUT/data/Arova/locale" \
    "$OUT/config" "$OUT/cache" "$OUT/icons" "$OUT/ssl/certs"

LIBDIR="$OUT/lib"

declare -a PENDING=()
declare -A SEEN=()

scan_binaries() {
    local f="$1"
    [ -f "$f" ] || return 0
    local lib
    while IFS= read -r lib; do
        [ -n "$lib" ] && [ -f "$lib" ] || continue
        if [ -z "${SEEN[$lib]+x}" ]; then
            SEEN[$lib]=1
            if is_gl_lib "$lib"; then
                continue
            fi
            cp -L -f "$lib" "$LIBDIR/" 2>/dev/null
            PENDING+=("$lib")
        fi
    done < <(ldd "$f" 2>/dev/null | awk '/=> \/.*\.so/{print $3} /^\t\/.*\.so/{print $1}')
}

process_pending() {
    local i=0
    local n
    while : ; do
        n=${#PENDING[@]}
        [ "$i" -lt "$n" ] || break
        scan_binaries "${PENDING[$i]}"
        i=$((i+1))
    done
}

scan_tree() {
    local root="$1"
    [ -d "$root" ] || return 0
    local so
    while IFS= read -r -d '' so; do
        scan_binaries "$so"
    done < <(find "$root" -type f -name "*.so" -print0 2>/dev/null)
}

echo "[1/9] Copiando el ejecutable y QtWebEngineProcess..."
cp -f "$AROVA_BIN" "$OUT/bin/arova"
cp -f "$WEBENGINE_PROCESS" "$OUT/libexec/QtWebEngineProcess"
scan_binaries "$AROVA_BIN"
scan_binaries "$WEBENGINE_PROCESS"

echo "[2/9] Copiando plugins de Qt..."
cp -a "$QT_PLUGINS_DIR/." "$OUT/plugins/"
scan_tree "$OUT/plugins"

echo "[3/9] Copiando modulos QML..."
cp -a "$QT_QML_DIR/." "$OUT/qml/"
scan_tree "$OUT/qml"

echo "[4/9] Recopilando todas las librerias dependientes..."
process_pending
for extra in libGLESv2.so.2 libEGL.so.1 libGL.so.1 libGLX.so.0 libGLdispatch.so.0 libvulkan.so.1; do
    is_gl_lib "$extra" && continue
    f="/lib/x86_64-linux-gnu/$extra"
    [ -f "$f" ] || f="/usr/lib/x86_64-linux-gnu/$extra"
    if [ -f "$f" ]; then
        cp -L -f "$f" "$LIBDIR/" 2>/dev/null
        scan_binaries "$f"
    fi
done
process_pending
# El backend TLS de Qt busca nombres sin version: creamos symlinks para que
# use las versiones empaquetadas en vez de las del sistema.
for v in libssl.so.3 libcrypto.so.3; do
    if [ -f "$LIBDIR/$v" ]; then
        ln -sf "$v" "$LIBDIR/${v%.3}"
    fi
done

echo "[5/9] Copiando traducciones y recursos de QtWebEngine..."
cp -a "$QT_TRANSLATIONS_DIR/." "$OUT/translations/"
cp -a "$QT_RESOURCES_DIR/." "$OUT/data/resources/"

echo "[6/9] Copiando traducciones de la aplicacion..."
cp -a "$APP_LOCALE_DIR/." "$OUT/bin/locale/"
cp -a "$APP_LOCALE_DIR/." "$OUT/data/Arova/locale/"

echo "[7/9] Copiando icono, fuentes y certificados..."
cp -f "$SCRIPT_DIR/src/data/128x128/arova.png" "$OUT/icons/"
if [ "$COPY_FONTS" = "1" ]; then
    mkdir -p "$OUT/fonts"
    for d in $FONTS_DIRS; do
        [ -d "$d" ] && cp -a "$d/." "$OUT/fonts/"
    done
fi
cp -a "$CERT_SRC/." "$OUT/ssl/certs/"

echo "[8/9] Escribiendo qt.conf y configuracion inicial..."
cat > "$OUT/bin/qt.conf" <<'QTCONF'
[Paths]
Prefix = ..
Plugins = plugins
Translations = translations
Data = data
LibraryExecutables = libexec
Qml2Imports = qml
QTCONF

if [ -f "$HOME/.config/GBonachea/Arova.conf" ]; then
    mkdir -p "$OUT/config/GBonachea"
    cp -f "$HOME/.config/GBonachea/Arova.conf" "$OUT/config/GBonachea/Arova.conf"
fi
if [ -d "$HOME/.local/share/Arova" ]; then
    cp -a "$HOME/.local/share/Arova/." "$OUT/data/Arova/"
fi

cat > "$OUT/fonts/fonts.conf" <<'FONTCONF'
<?xml version="1.0"?>
<!DOCTYPE fontconfig SYSTEM "fonts.dtd">
<fontconfig>
    <dir prefix="cwd">fonts</dir>
    <cachedir prefix="cwd">cache/fontconfig</cachedir>
</fontconfig>
FONTCONF

echo "[9/9] Escribiendo el lanzador..."
cat > "$OUT/Arova-portable.sh" <<'LAUNCHER'
#!/bin/bash
# Lanzador de Arova portable. Todo se carga desde esta carpeta.
APP_DIR="$(cd "$(dirname "$0")" && pwd)"

export LD_LIBRARY_PATH="$APP_DIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="$APP_DIR/plugins"
export QML2_IMPORT_PATH="$APP_DIR/qml"
export QTWEBENGINEPROCESS_PATH="$APP_DIR/libexec/QtWebEngineProcess"
export QTWEBENGINE_DISABLE_SANDBOX=1
export XDG_CONFIG_HOME="$APP_DIR/config"
export XDG_DATA_HOME="$APP_DIR/data"
export XDG_CACHE_HOME="$APP_DIR/cache"
export FONTCONFIG_FILE="$APP_DIR/fonts/fonts.conf"
export SSL_CERT_DIR="$APP_DIR/ssl/certs"
export SSL_CERT_FILE="$APP_DIR/ssl/certs/ca-certificates.crt"

cd "$APP_DIR"
exec "$APP_DIR/bin/arova" "$@"
LAUNCHER
chmod +x "$OUT/Arova-portable.sh" "$OUT/bin/arova"

echo ""
echo "Portable creado en: $OUT"
echo "Para ejecutarlo:"
echo "  $OUT/Arova-portable.sh"
echo "Librerias incluidas: $(ls "$OUT/lib" | wc -l)"
du -sh "$OUT"
