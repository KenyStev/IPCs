## Crear un Shared Memory
Necesitamos un `key_t` que debe ser unico, es una llave que va a identificar nuestro vector compartido
en este caso estamos usando el `pid_t` del padre como key

# tenemos:
```c
int shmid;
key_t key=0;
pid_t pid_s=0;
char *shm;
```

# creamos
```c
/*
 * Create the segment.
 */
 if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
      perror("shmget");
      exit(1);
 }

 /*
 * Now we attach the segment to our data space.
 */
 if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
      perror("shmat");
      exit(1);
 }
```
`shmget` recibe el `key_t`, `SHMSZ` es el tamaño en bytes, y las flags: `IPC_CREAT | 0666` para decirle que lo cree y que nos de permiso de escritura y lectura.

`shmat` recibe el identificador que nos devolvio `shmget`, `NULL` para decirle que nos asigne un nuevo apuntador donde comienza nuestra memoria compartida, `0` no recibe ningina flag.

ahora en `shm` tenemos la direccion donde comienza nuestra memoria.

cuando llamamos `shmget` estamos creando la memoria compartida.
cuando llamamos `shmat` estamos anclando nuestro proceso a ese espacio de memoria.

> para anclar otro proceso una vez creada la memoria compartida solo debemos saber el `key_t`
> y hacemos lo mismo que para crear, pero le quitamos solamente el flag: `IPC_CREAT` 
> y lo demas es exactamente igual.

# Desanclar
```c
shmdt(shm);
```
para desanclar nuestro proceso de la memoria compartida, solo necesitamos el apuntador donde esta comienza, que es el mismo que nos fue devuelto por: `shmat`.

# Borrar
```c
shmctl(shmid, IPC_RMID, NULL);
```
para borrar nuestra memoria compartida y liberar nustra RAM, solo necesitamos el identificador que nos fue devuelto por: `shmget`.

Fuente:
=======
> [Crear y Usar](http://users.cs.cf.ac.uk/Dave.Marshall/C/node27.html)
> [Borrar y Desanclar](http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/shmdt.html)