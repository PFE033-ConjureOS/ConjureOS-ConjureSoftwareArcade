# Utilisation du GUI Conjure Arcade

___

L'interface graphique de l'arcade Conjure est fait à partir d'un fork de l'
application [Pegasus Frontend](https://pegasus-frontend.org/) un application frontend pour naviguer à travers des listes
sur un appareil.

## Compiler le projet

Afin de compiler l'application on peut se fier à la documentation de pegasus:

- https://pegasus-frontend.org/docs/dev/build/

### Étapes

- Pull le repo (maybe avec -recursive pour aller chercher les submodule)
  > git clone --recursive <https://github.com/PFE033-ConjureOS/ConjureOS-SoftwareArcade.git>
- Télécharger Qt et faire l'installation de Qt 5.x (la license étudiant ou open source)
  > <https://www.qt.io/download>

  S'assurer que Qt est dans le PATH window. (Exemple de chemin ajouter au path : _C:\Qt\5.15.15\msvc2019_64\bin_)
- Build le projet sur Qt Creator avec le kit MinGW ou MSCV
- On peut lancer l'interface en exécutant le projet "app"

___

## Scripts pythons

Dans le sous dossier _script_python_, se trouve deux scripts python:
- **ConjureDecompression.py**
  - Ce script s'occupe d'extraire les jeux des fichiers _.conj_ au [chemin par défaut](#fichier-conj) vers le [dossier racine des jeux](#dossier-racine-des-jeux)
- **ConjureHttpPullGames.py**
  - Ce script télécharge les fichiers .conj qui n'existe pas dans le [dossier conj](#fichier-conj)
  - l'adresse du serveur doit être mis à jour dans le fichier _.env_ (la variable "DOMAIN")
___

## Chemins par défauts

### - Themes:
> C:\Users\<user>\AppData\Local\conjure\conjure-os\themes

Ici, on peut ajouter différents interchangeable rapidement. Si aucun existe, le theme _[conjure-theme](src/themes/conjure-theme)_ sera choisi par défaut.

### - fichier conj
> C:\Users\jonny\AppData\Local\conjure\conjure-os\conj

Ce dossier est lu par les [script pythons](#scripts-pythons)

### - Dossier racine des jeux
> C:\Users\<user>\Documents\ConjureGames

Dans se dossier, chaque jeux on leur propre metadata.txt qui est lui pas l'application pour l'afficher dans l'interface.
___

## Déploiement

Dans pegasus, le deploiement automatique est fait avec AppVeyor et le script exécuté est le .*appveyor.yml* à la racine du projet

Dans ce fichier, on voit un qu’il exécute *windeploy.exe* que l’on peut exécuté manuellement.

Voici un exemple:
### Release avec QT 5.15.16 MSVC:
> windeployqt --release --qmldir . --no-translations --no-opengl-sw C:\Users\jonny\Documents\_Ecole\_9e_session\___LOG795\repo\build-pegasus-Desktop_Qt_5_15_16_MSVC2019_64bit-Release\src\app\release


### Deploiement plus universel?
    cd [build_dir]
    windeployqt --release --qmldir . --no-translations --no-opengl-sw src\app\release

Pour m’aider, on peut ajouter un custom step directement dans QT Creator afin d’exécuter windeployqt et de copier les fichiers python dans le shadow build:
- **windeployqt**
  - --release --qmldir . --no-translations --no-opengl-sw src\app\release
  - %{buildDir}
- **xcopy**
  - %{sourceDir}\python_script %{buildDir}\src\app\release\python_script /E /I /c /y
  - %{sourceDir}

!! ATTENTION !! Pour une raison ou une autre, certain module QML ne se copie comme il faut lorsqu’on appel windeployqt des fois. Il faut parfois build recompiler en nettoyant les compilations.
OU littérallement copier coller les dlls.

### Intégration de SDL2
En suivant la [documentation de pegasus](https://pegasus-frontend.org/docs/dev/build/#:~:text=If%20SDL_LIBS%20and,to%20SDL2main), on peut ajouter les configurations pour compiler avec SDL directement dans

Donc, les argumets a ajouter à l'étape _make_ :
    

    USE_SDL_GAMEPAD=1 SDL_LIBS="-L<Path du dossier SDL2.dll> -lSDL2" SDL_INCLUDES=<Path des headers de la bibliothèque SDL2>

Exemple:

    USE_SDL_GAMEPAD=1 SDL_LIBS="-LC:\Users\jonny\Documents\SDL2\SDL_LIBS\Release -lSDL2" SDL_INCLUDES=C:\Users\jonny\Documents\SDL2\SDL-release-2.28.5\include

###  Updater l'arcade en tant que tel
Pour l’instant, après m’assurer d’avoir tout les dll (avec l’aide de windeployqt), je zip tout les fichier dans le dossier ou se retrouve *************conjure-os.ex.*************

Parfois, il manque des dll, c’est pas très stable, mais c’est fonctionnel.

___

## Problème rencontré

### 1. Error DoRender puis ca plante

Erreur lorsqu’il essaye de lire les video de preview. Cela veut dire qu'il manque un codec.
On peut installer le suivant et ca fonctionne:
> https://codecguide.com/download_kl.htm
