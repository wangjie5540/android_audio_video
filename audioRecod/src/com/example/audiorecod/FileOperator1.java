package com.example.audiorecod;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class FileOperator1 {
	private final static String path = "/sdcard/test";
	private static FileOutputStream fo;
	private static BufferedOutputStream bos = null;
	private static DataOutputStream dos;
	public static void append(short[] data, int length){
		try {
			fo = new FileOutputStream(path, true);
			bos = new BufferedOutputStream(fo);
			dos = new DataOutputStream(bos);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		try {
			for (int i = 0; i < length; i++){
				dos.writeShort(data[i]);
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		if (dos != null){
			try {
				dos.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
}
