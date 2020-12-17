import java.io.IOException;
import java.util.Scanner;
import com.fazecast.jSerialComm.*;

public class Serial {

    public static void main(String[] args) throws IOException {

        SerialPort[] ports = SerialPort.getCommPorts();
        System.out.println("Select a port:");
        int i = 1;
        for(SerialPort port : ports)
            System.out.println(i++ +  ": " + port.getSystemPortName());
        Scanner s = new Scanner(System.in);
        int chosenPort = s.nextInt();

        SerialPort serialPort = ports[chosenPort - 1];
        if(serialPort.openPort())
            System.out.println("Port opened successfully.");
        else {
            System.out.println("Unable to open the port.");
            return;
        }
        serialPort.setComPortParameters(9600, 8, 1, SerialPort.NO_PARITY);
        serialPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);

        /*Scanner data = new Scanner(serialPort.getInputStream());
        int value = 0;
        while(data.hasNextLine()){
            try{value = Integer.parseInt(data.nextLine());}catch(Exception e){}
            System.out.println(value);
        }
        System.out.println("Done.");*/

        serialPort.getOutputStream().write(123);

    }

}