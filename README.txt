Pour compiler le programme:
 placez vous dans le repertoire build et lancez la commande "cmake.."
un makefile sera généré, lancez le avec la commande make.

lancer le programme:
une fois compilé, placez vous dans le répertoire build et lancez "./TP_projet/TPprojet_projet"

Utilisation et commandes:
Une fois le programme lancé vous pouvez affecté le programme avec ces commandes:

z,q,s,d permettent de déplacer l'ovservateur
o,k,l,m permettent de pivoter la perspective, 
appuyé sur un bouton de la souris puis la déplacer a le même éffet, pour l'arréter pressez a nouveau un bouto

a et e permettent de ralentir et accélerer respectivement le mouvement de la bielle

p arrete la bielle, une nouvelle pression reprend la rotation a la vitesse lors de l'arrete

t,y g,h et v,b permmettent de modifier les données de la bielle, le premier augmente l'element associé et le second le diminue,
ils affectent les elements dans cet ordre : arbre, bras, épaisseur

les boutons du pavé numérique 0,1 et 2 permettent de toggle les différentes lumières de la scène
si vous n'avez pas de pavé numérique, r,f et c le font aussi

escape permet de quitter le programme

Un probleme a été découvert lors de la compilation sur les machines de l'université le fichier glm.hpp nécéssite la ligne : #define GLM_ENABLE_EXPERIMENTAL
Un second problème sur les machines de l'université est le non afichage de la seconde spotlight (censée suivre le piston) ainsi que l'impossibilité d'éteindre la lumière caméra, aucune solution n'est connue
