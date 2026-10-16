#!/bin/sh

# Exit on error.
set -e

MAGENTA="\033[1;35m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
CLEAR="\033[0m"


info() {
    printf "%b%s%b\n\n" "$MAGENTA" "$1" "$CLEAR"
}


success() {
    printf "%b%s%b\n\n" "$GREEN" "$1" "$CLEAR"
}


warn() {
    printf "%b[WARNING]: %s%b\n\n" "$YELLOW" "$1" "$CLEAR"
}


error() {
    printf "%b[ERROR]: %s%b\n\n" "$RED" "$1" "$CLEAR"
    exit 1
}


# Check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}


# Check if running as root (for package installation)
check_sudo() {
    if [[ $EUID -eq 0 ]]; then
        SUDO=""
    elif command_exists sudo; then
        SUDO="sudo"
    else
        error "This script requires sudo privileges to install packages"
    fi
}


detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        PRINTOS="macOS"
        return
    fi

    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        case "$ID" in
            ubuntu|debian|linuxmint|pop)
                OS="debian"
                PRINTOS="Debian/Ubuntu"
                ;;
            fedora)
                OS="fedora"
                PRINTOS="Fedora"
                ;;
            centos|rhel|rocky|almalinux)
                OS="centos"
                PRINTOS="CentOS/RHEL"
                ;;
            arch|manjaro|endeavouros)
                OS="arch"
                PRINTOS="Arch Linux (btw)"
                ;;
            *)
                # Check for derivatives
                if [[ "$ID_LIKE" == *"debian"* ]]; then
                    OS="debian"
                    PRINTOS="Debian/Ubuntu"
                elif [[ "$ID_LIKE" == *"fedora"* ]] || [[ "$ID_LIKE" == *"rhel"* ]]; then
                    OS="fedora"
                    PRINTOS="Fedora"
                elif [[ "$ID_LIKE" == *"arch"* ]]; then
                    OS="arch"
                    PRINTOS="Arch Linux (btw)"
                else
                    OS="unknown"
                    PRINTOS="unknown"
                fi
                ;;
        esac
    else
        OS="unknown"
        PRINTOS="unknown"
    fi
}


install_gojo() {
    case "$OS" in
        macos)
            ROS="macos" 
            ;;
        debian)
            ROS="linux"
            ;;
        fedora)
            ROS="linux"
            ;;
        centos)
            ROS="linux"
            ;;
        arch)
            ROS="linux"
            ;;
        unknown)
            error "Unsupported operating system."
            ;;
    esac

    case "$(uname -m)" in
        x86_64)
            ARCH="amd64"
            ;;
        aarch64|arm64)
            ARCH="arm64"
            ;;
        unknown)
            error "Unsupported architecture."
            ;;
    esac

    info "Installing gojo from github..."
    # Download latest release from github into ~/.gojo/bin
    curl --proto '=https' --tlsv1.2 -sSf -L "https://github.com/whuffman36/gojo/releases/download/v0.1.5/gojo-${ROS}-${ARCH}" > "$HOME/.gojo/bin/gojo"
    
    # Set binary to be executable
    chmod +x "$HOME/.gojo/bin/gojo"

    # Download gojo-update from github into ~/.gojo/bin
}


# Install dependencies on macOS
install_macos() {
    if ! command_exists brew; then
        error "Homebrew is required. Install it from https://brew.sh"
    fi

    info "Installing gojo dependencies via Homebrew..."

    if ! xcode-select -p; then
        xcode-select --install
    fi
    
    brew install -y cmake llvm clang-format cppcheck conan

    echo "\n"
    success "Dependencies installed"

    install_gojo

    # Add ~/.gojo/bin to PATH
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> "$HOME/.zshrc"

    success "gojo has successfully been installed!"
    printf "run '%bsource ~/.zshrc%b' in your terminal to use it,\nor start a new terminal session.\n\n" "$YELLOW" "$CLEAR"
}


# Install dependencies on Debian/Ubuntu
install_debian() {
    check_sudo

    info "Updating package lists..."
    $SUDO apt-get update -qq

    info "Installing gojo dependencies..."
    $SUDO apt-get install -y \
        cmake \
        clang-tidy \
        clang-format \
        cppcheck \
        pipx \
        build-essential

    pipx ensurepath
    source "$HOME/.bashrc"

    pipx install conan

    success "Dependencies installed"

    install_gojo

    # Add ~/.gojo/bin to PATH
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> "$HOME/.bashrc"

    success "gojo has successfully been installed!"
    printf "run '%bsource ~/.bashrc%b' in your terminal to use it,\nor start a new terminal session.\n\n", "$YELLOW" "$CLEAR"
}


# Install dependencies on Fedora
install_fedora() {
    check_sudo

    info "Installing gojo dependencies..."
    $SUDO dnf install -y \
        cmake \
        clang-tools-extra \
        cppcheck \
        gcc \
        pipx \
        gcc-c++

    pipx ensurepath
    source "$HOME/.bashrc"

    pipx install conan

    success "Dependencies installed"

    install_gojo

    # Add ~/.gojo/bin to PATH
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> "$HOME/.bashrc"

    success "gojo has successfully been installed!"
    printf "run '%bsource ~/.bashrc%b' in your terminal to use it,\nor start a new terminal session.\n\n", "$YELLOW" "$CLEAR"
}


# Install dependencies on CentOS/RHEL
# Honestly idk much about this distro, I totally copied this code.
install_centos() {
    check_sudo

    # Check CentOS version
    if [[ -f /etc/centos-release ]]; then
        CENTOS_VERSION=$(rpm -E %{rhel})
    else
        CENTOS_VERSION=$(rpm -E %{rhel} 2>/dev/null || echo "8")
    fi

    if [[ "$CENTOS_VERSION" -ge 8 ]]; then
        info "Enabling PowerTools/CRB repository..."
        $SUDO dnf install -y epel-release
        # CentOS 8 Stream / RHEL 8
        $SUDO dnf config-manager --set-enabled powertools 2>/dev/null || \
        $SUDO dnf config-manager --set-enabled crb 2>/dev/null || \
        warn "Could not enable PowerTools/CRB repo"

        info "Installing gojo dependencies..."
        $SUDO dnf install -y \
            cmake \
            clang-tools-extra \
            cppcheck \
            gcc \
            pipx \
            gcc-c++

        pipx ensurepath
        source "$HOME/.bashrc"

        pipx install conan
    else
        error "CentOS/RHEL 7 or earlier is not supported. Please upgrade to version 8+."
    fi

    success "Dependencies installed"

    install_gojo

    # Add ~/.gojo/bin to PATH
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> "$HOME/.bashrc"

    success "gojo has successfully been installed!"
    printf "run '%bsource ~/.bashrc%b' in your terminal to use it,\nor start a new terminal session.\n\n", "$YELLOW" "$CLEAR"
}


# Install dependencies on Arch Linux
install_arch() {
    check_sudo

    info "Installing gojo dependencies..."
    $SUDO pacman -Sy --needed --noconfirm \
        cmake \
        clang \
        cppcheck \
        python-pipx \
        base-devel

    pipx ensurepath
    source "$HOME/.bashrc"

    pipx install conan

    success "Dependencies installed"

    install_gojo

    # Add ~/.gojo/bin to PATH
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> "$HOME/.bashrc"

    success "gojo has successfully been installed!"
    printf "run '%bsource ~/.bashrc%b' in your terminal to use it,\nor start a new terminal session.\n\n", "$YELLOW" "$CLEAR"
}


main() {
    # Create directories in the user's home directory.
    mkdir -p "$HOME/.gojo"
    mkdir -p "$HOME/.gojo/profiles"
    mkdir -p "$HOME/.gojo/bin"

    detect_os
    info "$PRINTOS detected"

    case "$OS" in
        macos)
            install_macos
            ;;
        debian)
            install_debian
            ;;
        fedora)
            install_fedora
            ;;
        centos)
            install_centos
            ;;
        arch)
            install_arch
            ;;
        unknown)
            error "Unsupported operating system."
            ;;
    esac
}


main "$@"
