# Utilisation Pegasus Frontend

___

## Building Pegasus

https://pegasus-frontend.org/docs/dev/build/

### Étapes

- Pull le repo (maybe avec -recursive pour aller chercher les submodule)
  > git clone --recursive <https://github.com/PFE033-ConjureOS/ConjureOS-SoftwareArcade.git>
- Télécharger Qt et faire l'installation de Qt 5.x (la license étudiant ou open source)
  ><https://www.qt.io/download>

  J'ai essayer avec Qt6.x et je n'arrivais pas a build
- Build le projet sur Qt Creator comme indiqué dans la doc de pegasus

___

J'ai essayer aussi de build juste avec Cmake puis l'ouvrir dans Visual studio, mais il faut indiqué plein de référence a Qt et et autre module tandis que Qt creator fonctionne direct.