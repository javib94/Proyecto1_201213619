#Se ejecutará este script con
#exec "/home/javierb/github/Proyecto1_201213619/ejProyecto1/prueba.sh"

#Crea cuatro discos del mismo tamaño (60 Mb) pero en diferentes unidades
mkdisk -Size::60 -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk"
mkdisk -Size::61440 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk"
mkDisk -size::60 -unit::M -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D3.dsk"  
mkdisk -Size::61440 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D4.dsk"  


#Validaciones de mkdisk con datos no validos
mkDisk -size::-50 -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D5.dsk"
mkDisk -size::50 -unit::x -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D5.dsk"  


#Elimina un disco, Quedan 3 discos
#El primero debería dar error ya que no se creó
rmDisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D5.dsk"
rmDisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D4.dsk"


#Crear 11 particiones del mismo tamaño en el primer disco con ajustes diferentes
#Todas son de 5 Mb (La extendida es de 40 Mb)
#Tipo::		Primaria	Primaria	Extendida	Logica	Logica	Logica	Logica	Logica	Logica	Logica	Primaria
#Ajuste::	Primer 		Mejor		-			Peor	Peor	Peor	Peor	Peor	Peor	Peor	Peor
#Nombre::	PRI 1		PRI 2		EXT			LOG 1	LOG 2	LOG 3	LOG 4	LOG 5	LOG 6	LOG 7	PRI 3
fdisk -Size::5 -unit::M -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::P -fit::F -name::PRI1
fdisk -Size::5120 -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -fit::B -name::PRI2
fdisk -Size::40960 -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::E -name::EXT
fdisk -Size::5120 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L  -name::LOG1 
fdisk -Size::5120 -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L -name::LOG2  
fdisk -Size::5120 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L -name::LOG3
fdisk -Size::5120 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L -name::LOG4 
fdisk -Size::5120 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L -name::LOG5 
fdisk -Size::5120 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L -name::LOG6 
fdisk -Size::5120 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L -name::LOG7 
fdisk -Size::5242880 -unit::B -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::PRI3

#Verificar generacion de ids al montar
#Debería generar el ids:: vda1
mount -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::PRI3  
mount -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk" -name::PRI3 
mount -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::PRI3  
mount -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk" -name::PRI2
mount -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::PRI2 
#Validaciones al montar, no deberia cargarlos
mount -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk" -name::PX
mount -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/DX.dsk" -name::PRI3
#Reporte de las particiones del disco


#Debe crear la carpeta de reportes
#rep -name::disk -path::"/home/javierb/Documentos/Reporte1Disco2.png" -id::vda1
#rep -name::disk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/reportes/Reporte 1 Disco1.jpg" -id::vdb1  

#REVISAR HASTA AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
#Verificar eliminación
#Elimina la primaria 2, logica 1, lógica 2 y lógica 5 del disco 1 
#Queda así:: P1 EXT L2 L3 L6 L7 P3
fdisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -delete::Fast -name::PRI2 
fdisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -delete::Fast -name::LOG1
fdisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -delete::Full -name::LOG4  
fdisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -delete::Fast -name::LOG5
#Tambien elimina la partición extendida y la primaria 1 del disco 2 
#Verificar que no queden EBRs
#Queda así:: P2 P3
fdisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk" -delete::Full -name::EXT
fdisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk" -delete::Full -name::PRI1


#Reporte con particiones eliminadas
#rep -name::disk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/reportes/Reporte 2 Disco2.jpg" -id::vda1
#rep -name::disk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/reportes/Reporte 2 Disco1.jpg" -id::vdb1

#REVISAR HASTA AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
#Teoria de particiones, (solo una extendida) debería dar error
fdisk -Size::1024 -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::E -name::EXT2


#Verificar primer ajuste creando de nuevo las particiones
#Debe quedar de la siguiente forma::
#Disco1:: P1 P2 EXT L1 L2 L3 L45 L6 L7 P3
fdisk -Size::5 -unit::M  -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::P -fit::F -name::PRI2 
fdisk -Size::10240 -unit::K  -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L -name::LOG45 
fdisk -Size::5120 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::L -name::LOG1 
#Disco 2:: P2 EXT L1 P1 P3
fdisk -Size::10240 -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk" -type::E -name::EXT
fdisk -Size::5120 -unit::K -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk" -type::L -name::LOG1
fdisk -Size::10240 -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D2.dsk" -type::P -name::PRI1


#Teoria de particiones, máximo 4 entre primarias y extendidas, no debería permitir crear otra
fdisk -Size::5 -unit::M -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -type::P -fit::F -name::P4


#Reporte de las particiones del disco
#rep -name::disk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/reportes/Reporte 3 Disco2.jpg" -id::vda1 
#rep -name::disk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/reportes/Reporte 3 Disco1.jpg" -id::vdb1  
#REVISAR HASTA AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

#Validaciones al crear o quitar espacio
#No deberia ejecutar estas instrucciones
#(porque no hay espacio después)
fdisk -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::PRI1 -add::1
#(porque no debe quedar espacio negativo)
fdisk -unit::M -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::PRI1 -add::-20
#(Porque no hay suficiente espacio)
fdisk -unit::M -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::LOG7 -add::15

x|
#Quita y agrega 1 mb de la primer particion
fdisk -unit::M -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::PRI1 -add::-1
rep -name::Mbr -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/reportes/Reporte 1 MBR Disco 1.jpg" -id::vdb2
fdisk -unit::M -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::PRI1 -add::1
rep -name::Mbr -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/reportes/Reporte 2 MBR Disco 1.jpg" -id::vdb2
#Le quita 1 mb a la partición logica
fdisk -unit::M -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/D1.dsk" -name::LOG45 -add::-1
rep -name::Mbr -path::"/home/javierb/github/Proyecto1_201213619/ejProyecto1/reportes/Reporte 3 MBR Disco 1.jpg" -id::vdb2
