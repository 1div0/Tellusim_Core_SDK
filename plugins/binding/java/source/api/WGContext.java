// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

package com.tellusim;
import com.tellusim.*;
import java.lang.ref.Cleaner;

/*
 */
public class WGContext extends Context {
	
	public WGContext() { init_(new_()); }
	public WGContext[] ref() { return new WGContext[] { this }; }
	public WGContext(Context base) { init_(cast_context_ptr(base.self)); }
	public Context toContext() { return new Context(base_context_ptr(self)); }
	public static boolean open(long instance, long adapter, long device) { return open_(instance, adapter, device); }
	public boolean create(long instance, long adapter, long device) { return create_(self, instance, adapter, device); }
	public long getInstance() { return get_instance(self); }
	public long getAdapter() { return get_adapter(self); }
	public long getDevice() { return get_device(self); }
	
	private static native long new_();
	private static native void delete_(long self);
	private static native boolean equal_context_ptr(long self, long base);
	private static native long cast_context_ptr(long self);
	private static native long base_context_ptr(long self);
	private static native boolean open_(long instance, long adapter, long device);
	private static native boolean create_(long self, long instance, long adapter, long device);
	private static native long get_instance(long self);
	private static native long get_adapter(long self);
	private static native long get_device(long self);
	
	protected WGContext(long self) {
		init_(self);
	}
	public static WGContext Null() {
		return new WGContext((long)0);
	}
}
