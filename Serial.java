import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;
import com.fazecast.jSerialComm.*;

public class Serial {

    public static void main(String[] args) throws IOException {

        System.out.println("connect");
        SerialPort port = SerialPort.getCommPort("COM6"); // change if different
        port.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER,0,0);
        port.openPort();
        port.setBaudRate(9600);

        System.out.println(port.getBaudRate());

        try {Thread.sleep(600); } catch(Exception e) {} // I think this sleep is a must until it opens
        System.out.println(port.isOpen());
        while(true){
            if(port.isOpen()) {
                System.out.println("port open!");
                //try {Thread.sleep(500); } catch(Exception e) {}

                File file=new File("C:\\Users\\robi\\Desktop\\proiect_v1\\comm\\src\\info");
                Scanner reader=new Scanner(file);
                String info= reader.nextLine();

                PrintWriter output = new PrintWriter(port.getOutputStream());
                output.write(info);
                try {Thread.sleep(500); } catch(Exception e) {}
                output.flush();
                System.out.println("info sent");

                try {Thread.sleep(500); } catch(Exception e) {}
                //port.closePort();
                Scanner data = new Scanner(port.getInputStream());
                String value="";
                if(data.hasNextLine()){
                    value = data.nextLine();
                    FileWriter fileWriter=new FileWriter(file);
                    PrintWriter printWriter=new PrintWriter(fileWriter);
                    printWriter.print(value);
                    printWriter.close();
                    System.out.println(value);
                }
            }
        }


    }

}