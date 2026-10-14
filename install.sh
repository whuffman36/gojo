#!/bin/bash

# Exit on error.
set -e

MAGENTA="\033[1;35m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
CLEAR="\033[0m"

info() {
    echo -e "${MAGENTA}$1${CLEAR}"
}

success() {
    echo -e "${GREEN}$1${CLEAR}"
}

warn() {
    echo -e "${YELLOW}$1${CLEAR}"
}

error() {
    echo -e "${RED}[ERROR]: $1${CLEAR}"
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
        OS="macOS"
        return
    fi

    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        case "$ID" in
            ubuntu|debian|linuxmint|pop)
                OS="Debian/Ubuntu based system"
                ;;
            fedora)
                OS="Fedora"
                ;;
            centos|rhel|rocky|almalinux)
                OS="CentOS/RHEL"
                ;;
            arch|manjaro|endeavouros)
                OS="Arch Linux"
                ;;
            *)
                # Check for derivatives
                if [[ "$ID_LIKE" == *"debian"* ]]; then
                    OS="Debian/Ubuntu based system"
                elif [[ "$ID_LIKE" == *"fedora"* ]] || [[ "$ID_LIKE" == *"rhel"* ]]; then
                    OS="Fedora based system"
                elif [[ "$ID_LIKE" == *"arch"* ]]; then
                    OS="Arch based system"
                else
                    OS="unknown"
                fi
                ;;
        esac
    else
        OS="unknown"
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

    # Download latest release from github into ~/.gojo/bin
    curl --proto '=https' --tlsv1.2 -sSf -L "https://github.com/whuffman36/gojo/releases/download/v0.1.2/gojo-${ROS}-${ARCH}" > "$HOME/.gojo/bin/gojo"

    # Download gojo-update from github into ~/.gojo/bin
}

# Install dependencies on macOS
install_macos() {
    if ! command_exists brew; then
        error "Homebrew is required. Install it from https://brew.sh"
    fi

    info "Installing gojo dependencies via Homebrew...\n"

    if ! xcode-select -p; then
        xcode-select --install
    fi
    
    brew install cmake llvm clang-format cppcheck conan

    success "Dependencies installed\n"

    install_gojo

    # Add ~/.gojo/bin to PATH
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> ~/.zshrc

    success "gojo has successfully been installed!\n"
    echo -e "run '${YELLOW}source ~/.zshrc${CLEAR}' in your terminal to use it,\n"
    echo -e "or start a new terminal session.\n"
}

# Install dependencies on Debian/Ubuntu
install_debian() {
    check_sudo

    info "Updating package lists...\n"
    $SUDO apt-get update -qq

    info "Installing gojo dependencies...\n"
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

    success "Dependencies installed\n"

    install_gojo

    # Add ~/.gojo/bin to PATH
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> ~/.bashrc

    success "gojo has successfully been installed!\n"
    echo -e "run '${YELLOW}source ~/.bashrc${CLEAR}' in your terminal to use it,\n"
    echo -e "or start a new terminal session.\n"
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
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> ~/.bashrc

    success "gojo has successfully been installed!\n"
    echo -e "run '${YELLOW}source ~/.bashrc${CLEAR}' in your terminal to use it,\n"
    echo -e "or start a new terminal session.\n"
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
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> ~/.bashrc

    success "gojo has successfully been installed!\n"
    echo -e "run '${YELLOW}source ~/.bashrc${CLEAR}' in your terminal to use it,\n"
    echo -e "or start a new terminal session.\n"
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
    echo "export PATH=\"\$PATH:\$HOME/.gojo/bin/\"" >> ~/.bashrc

    success "gojo has successfully been installed!\n"
    echo -e "run '${YELLOW}source ~/.bashrc${CLEAR}' in your terminal to use it,\n"
    echo -e "or start a new terminal session.\n"
}

main() {
    # Create directories in the user's home directory.
    mkdir -p "$HOME/.gojo"
    mkdir -p "$HOME/.gojo/profiles"
    mkdir -p "$HOME/.gojo/bin"

    detect_os
    info "Detected OS: $OS"

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
