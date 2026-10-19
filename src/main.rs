#![no_std]
#![no_main]

mod memoria;
mod vga_buffer;

use core::panic::PanicInfo;

#[unsafe(no_mangle)]
pub extern "C" fn _start() -> ! {
    println!("so1_kernel UMG - Asignacion de memoria simple");
    let mut m = memoria::Memoria::new();
    m.mostrar();

    // 1. Tres procesos piden memoria
    let a = m.reservar(b'A', 40).unwrap();
    let b = m.reservar(b'B', 100).unwrap();
    let c = m.reservar(b'C', 20).unwrap();
    m.mostrar();

    // 2. Escribir y leer
    m.escribir(a, 0, b"Hola UMG").unwrap();
    if let Ok(datos) = m.leer(a, 0, 8) {
        println!("A contiene: {}", core::str::from_utf8(datos).unwrap());
    }

    // 3. Proteccion de Escritura
    match m.escribir(c, 30, b"desborde!") {
        Ok(_) => println!("Escritura en C aceptada"),
        Err(e) => println!("Escritura en C rechazada: {}", e),
    }

    // Reto A: Proteccion de Lectura
    match m.leer(a, 35, 20) {
        Ok(datos) => println!("Lectura en A aceptada: {}", core::str::from_utf8(datos).unwrap()),
        Err(e) => println!("Lectura en A rechazada: {}", e),
    }

    // 4. Liberar B deja un hueco
    m.liberar(b);
    m.mostrar();

    // Reto B: Mejor Ajuste con D
    let _d = m.reservar_mejor_ajuste(b'D', 50);
    let _e = m.reservar(b'E', 700);
    m.mostrar();

    // 5. Intentar F (Falla por Fragmentacion Externa)
    let _f = m.reservar(b'F', 150);

    // Reto C: Compactacion
    m.compactar();
    m.mostrar();
    let _f = m.reservar(b'F', 150);
    m.mostrar();

    loop {}
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    println!("{}", info);
    loop {}
}
