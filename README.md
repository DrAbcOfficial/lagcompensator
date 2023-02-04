# `LagCompensator`: Plugin to add lag compensator for Sven Co-op <img align="right" src="./img/logo.png" width="140" height="140" alt="LagCompensator" />

[![License: GPL v3](https://img.shields.io/github/license/DrAbcOfficial/lagCompensator)](https://www.gnu.org/licenses/gpl-3.0)
[![Build](https://img.shields.io/github/workflow/status/DrAbcOfficial/lagCompensator/Build)](https://github.com/DrAbcOfficial/asextlagCompensatorhook/actions)
[![Release](https://img.shields.io/github/v/release/DrAbcOfficial/lagCompensator)](https://github.com/DrAbcOfficial/lagCompensator/releases)
[![Download](https://img.shields.io/github/downloads/DrAbcOfficial/lagCompensator/total)](https://github.com/DrAbcOfficial/lagCompensator/releases)
[![Commit](https://img.shields.io/github/last-commit/DrAbcOfficial/lagCompensator)](https://github.com/DrAbcOfficial/lagCompensator)

`lagCompensator` is a Metamod plugin used to add lag compensator for the Sven Co-op.

---

<!-- vscode-markdown-toc -->
* 1. [Install](#Install)
* 2. [Build](#Build)
<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

---

##  1. <a name='Install'></a>Install

1. Grab lagCompensator.dll/lagCompensator.so, put them into `svencoop/addons/metamod/dlls`
   1.  Grab latest version in [Action](https://github.com/DrAbcOfficial/lagCompensator/actions/workflows/build.yml) (Unstable, crash risk)
		
		or

   2.  Grab release version in [Release](https://github.com/DrAbcOfficial/lagCompensator/releases) (Stable)
2. edit `svencoop/addons/metamod/plugins.ini`
3. add 

``` ini

win32 addons/metamod/dlls/lagCompensator.dll
linux addons/metamod/dlls/lagCompensator.so

```

4. Set `sv_unlag 1`

5. enjoy  <:3

---

##  2. <a name='Build'></a>Build

If you are using systems that are not covered by automatic builds (yum's and aur's) 

or want to add new hooks yourself, you can follow these steps to configure your build environment.

1. Install everything you need
   
	<img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"/>

	1. [Git](https://git-scm.com/download/win) or `winget install --id Git.Git -e --source winget`
	2. [Visual Studio with vc143 toolset](https://visualstudio.microsoft.com/) And [C++ desktop development](https://learn.microsoft.com/en-us/cpp/ide/using-the-visual-studio-ide-for-cpp-desktop-development?view=msvc-170)

	<img src="https://img.shields.io/badge/Debian-A81D33?style=for-the-badge&logo=debian&logoColor=white"/>

	1. `sudo apt install git make build-essential gcc gcc-multilib g++-multilib -y`

	<img src="https://img.shields.io/badge/Fedora-294172?style=for-the-badge&logo=fedora&logoColor=white"/>

	1. `sudo yum install git make gcc gcc-c++ glibc-devel.i686 libstdc++-devel.i686 -y`

	<img src="https://img.shields.io/badge/Arch_Linux-1793D1?style=for-the-badge&logo=arch-linux&logoColor=white"/>

	Sorry guys who using Arch, I've never used an Arch based distribution, you'll have to find out how to configure the dependencies yourselves!🙂

2. Clone metamod modified by hzqst
   
   `git clone https://github.com/hzqst/metamod-fallguys.git metamod`

   `cd metamod`

3. Clone this

	`git clone https://github.com/DrAbcOfficial/lagCompensator.git`

	`cd lagCompensator`

4. Build!

	<img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"/>

	1. Open `lagCompensator.sln` with visual studio
	2. Press `F7`
	3. Wait and done.

	<img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black"/>

	1. `make OPT=opt install`
   
6. Grab
	
	Now you can grab your new library (lagCompensator.dll/lagCompensator.so) in 
	
	`metamod/build/addons/metamod/dlls/`