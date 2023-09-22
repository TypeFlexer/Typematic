; ModuleID = 'callback.c'
source_filename = "callback.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@sbxHeap = common global i64 0
@sbxHeapRange = common global i32 0
@.str = private unnamed_addr constant [12 x i8] c"Result: %s\0A\00", align 1
@__const.main.buffer1 = private unnamed_addr constant [50 x i8] c"Hello, \00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", align 16

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @concatenate_tainted_strings(i8* %str1, i8* %str2, i8* %result) #0 {
entry:
  %str1.addr = alloca i8*, align 4
  %str2.addr = alloca i8*, align 4
  %result.addr = alloca i8*, align 4
  %0 = ptrtoint i8* %str1 to i32
  %1 = zext i32 %0 to i64
  %2 = bitcast i8** %str1.addr to i64*
  store i64 %1, i64* %2, align 4
  %3 = ptrtoint i8* %str2 to i32
  %4 = zext i32 %3 to i64
  %5 = bitcast i8** %str2.addr to i64*
  store i64 %4, i64* %5, align 4
  %6 = ptrtoint i8* %result to i32
  %7 = zext i32 %6 to i64
  %8 = bitcast i8** %result.addr to i64*
  store i64 %7, i64* %8, align 4
  %9 = load i8*, i8** %result.addr, align 4
  %10 = load i8*, i8** %str1.addr, align 4
  %11 = load i32, i32* @sbxHeapRange, align 8
  %12 = load i64, i64* @sbxHeap, align 8
  %13 = ptrtoint i8* %9 to i32
  %14 = zext i32 %13 to i64
  %15 = add i64 %12, %14
  %16 = icmp ult i32 %13, %11
  br i1 %16, label %_Dynamic_check.succeeded, label %_Dynamic_check.failed

_Dynamic_check.succeeded:                         ; preds = %entry
  %17 = inttoptr i64 %15 to i8*
  %18 = load i32, i32* @sbxHeapRange, align 8
  %19 = load i64, i64* @sbxHeap, align 8
  %20 = ptrtoint i8* %10 to i32
  %21 = zext i32 %20 to i64
  %22 = add i64 %19, %21
  %23 = icmp ult i32 %20, %18
  br i1 %23, label %_Dynamic_check.succeeded2, label %_Dynamic_check.failed1

_Dynamic_check.succeeded2:                        ; preds = %_Dynamic_check.succeeded
  %24 = inttoptr i64 %22 to i8*
  %call = call i8* @strcpy(i8* %17, i8* %24) #6
  %25 = load i8*, i8** %result.addr, align 4
  %26 = load i8*, i8** %str2.addr, align 4
  %27 = load i32, i32* @sbxHeapRange, align 8
  %28 = load i64, i64* @sbxHeap, align 8
  %29 = ptrtoint i8* %25 to i32
  %30 = zext i32 %29 to i64
  %31 = add i64 %28, %30
  %32 = icmp ult i32 %29, %27
  br i1 %32, label %_Dynamic_check.succeeded4, label %_Dynamic_check.failed3

_Dynamic_check.succeeded4:                        ; preds = %_Dynamic_check.succeeded2
  %33 = inttoptr i64 %31 to i8*
  %34 = load i32, i32* @sbxHeapRange, align 8
  %35 = load i64, i64* @sbxHeap, align 8
  %36 = ptrtoint i8* %26 to i32
  %37 = zext i32 %36 to i64
  %38 = add i64 %35, %37
  %39 = icmp ult i32 %36, %34
  br i1 %39, label %_Dynamic_check.succeeded6, label %_Dynamic_check.failed5

_Dynamic_check.succeeded6:                        ; preds = %_Dynamic_check.succeeded4
  %40 = inttoptr i64 %38 to i8*
  %call7 = call i8* @strcat(i8* %33, i8* %40) #6
  ret void

_Dynamic_check.failed:                            ; preds = %entry
  call void @llvm.trap() #7
  unreachable

_Dynamic_check.failed1:                           ; preds = %_Dynamic_check.succeeded
  call void @llvm.trap() #7
  unreachable

_Dynamic_check.failed3:                           ; preds = %_Dynamic_check.succeeded2
  call void @llvm.trap() #7
  unreachable

_Dynamic_check.failed5:                           ; preds = %_Dynamic_check.succeeded4
  call void @llvm.trap() #7
  unreachable
}

; Function Attrs: nounwind
declare dso_local i8* @strcpy(i8*, i8*) #1

; Function Attrs: cold noreturn nounwind
declare void @llvm.trap() #2

; Function Attrs: nounwind
declare dso_local i8* @strcat(i8*, i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @copy_tainted_string(i8* %str1, i8* %str2, i8* %result) #0 {
entry:
  %str1.addr = alloca i8*, align 4
  %str2.addr = alloca i8*, align 4
  %result.addr = alloca i8*, align 4
  %0 = ptrtoint i8* %str1 to i32
  %1 = zext i32 %0 to i64
  %2 = bitcast i8** %str1.addr to i64*
  store i64 %1, i64* %2, align 4
  %3 = ptrtoint i8* %str2 to i32
  %4 = zext i32 %3 to i64
  %5 = bitcast i8** %str2.addr to i64*
  store i64 %4, i64* %5, align 4
  %6 = ptrtoint i8* %result to i32
  %7 = zext i32 %6 to i64
  %8 = bitcast i8** %result.addr to i64*
  store i64 %7, i64* %8, align 4
  %9 = load i8*, i8** %result.addr, align 4
  %10 = load i8*, i8** %str1.addr, align 4
  %11 = load i32, i32* @sbxHeapRange, align 8
  %12 = load i64, i64* @sbxHeap, align 8
  %13 = ptrtoint i8* %9 to i32
  %14 = zext i32 %13 to i64
  %15 = add i64 %12, %14
  %16 = icmp ult i32 %13, %11
  br i1 %16, label %_Dynamic_check.succeeded, label %_Dynamic_check.failed

_Dynamic_check.succeeded:                         ; preds = %entry
  %17 = inttoptr i64 %15 to i8*
  %18 = load i32, i32* @sbxHeapRange, align 8
  %19 = load i64, i64* @sbxHeap, align 8
  %20 = ptrtoint i8* %10 to i32
  %21 = zext i32 %20 to i64
  %22 = add i64 %19, %21
  %23 = icmp ult i32 %20, %18
  br i1 %23, label %_Dynamic_check.succeeded2, label %_Dynamic_check.failed1

_Dynamic_check.succeeded2:                        ; preds = %_Dynamic_check.succeeded
  %24 = inttoptr i64 %22 to i8*
  %call = call i8* @strcpy(i8* %17, i8* %24) #6
  ret void

_Dynamic_check.failed:                            ; preds = %entry
  call void @llvm.trap() #7
  unreachable

_Dynamic_check.failed1:                           ; preds = %_Dynamic_check.succeeded
  call void @llvm.trap() #7
  unreachable
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @print_result_callback(i8* %result) #0 {
entry:
  %result.addr = alloca i8*, align 4
  %0 = ptrtoint i8* %result to i32
  %1 = zext i32 %0 to i64
  %2 = bitcast i8** %result.addr to i64*
  store i64 %1, i64* %2, align 4
  %3 = load i8*, i8** %result.addr, align 4
  %4 = load i32, i32* @sbxHeapRange, align 8
  %5 = load i64, i64* @sbxHeap, align 8
  %6 = ptrtoint i8* %3 to i32
  %7 = zext i32 %6 to i64
  %8 = add i64 %5, %7
  %9 = icmp ult i32 %6, %4
  br i1 %9, label %_Dynamic_check.succeeded, label %_Dynamic_check.failed

_Dynamic_check.succeeded:                         ; preds = %entry
  %10 = inttoptr i64 %8 to i8*
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i64 0, i64 0), i8* %10)
  ret void

_Dynamic_check.failed:                            ; preds = %entry
  call void @llvm.trap() #7
  unreachable
}

declare dso_local i32 @printf(i8*, ...) #3

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @perform_tainted_operation_with_callback(void (i8*, i8*, i8*)* %op, i8* %str1, i8* %str2, i8* %result, void (i8*)* %callback) #0 {
entry:
  %op.addr = alloca void (i8*, i8*, i8*)*, align 8
  %str1.addr = alloca i8*, align 4
  %str2.addr = alloca i8*, align 4
  %result.addr = alloca i8*, align 4
  %callback.addr = alloca void (i8*)*, align 8
  store void (i8*, i8*, i8*)* %op, void (i8*, i8*, i8*)** %op.addr, align 8
  %0 = ptrtoint i8* %str1 to i32
  %1 = zext i32 %0 to i64
  %2 = bitcast i8** %str1.addr to i64*
  store i64 %1, i64* %2, align 4
  %3 = ptrtoint i8* %str2 to i32
  %4 = zext i32 %3 to i64
  %5 = bitcast i8** %str2.addr to i64*
  store i64 %4, i64* %5, align 4
  %6 = ptrtoint i8* %result to i32
  %7 = zext i32 %6 to i64
  %8 = bitcast i8** %result.addr to i64*
  store i64 %7, i64* %8, align 4
  store void (i8*)* %callback, void (i8*)** %callback.addr, align 8
  %9 = load void (i8*, i8*, i8*)*, void (i8*, i8*, i8*)** %op.addr, align 8
  %10 = load i8*, i8** %str1.addr, align 4
  %11 = load i8*, i8** %str2.addr, align 4
  %12 = load i8*, i8** %result.addr, align 4
  call void %9(i8* %10, i8* %11, i8* %12)
  %13 = load void (i8*)*, void (i8*)** %callback.addr, align 8
  %14 = load i8*, i8** %result.addr, align 4
  call void %13(i8* %14)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %buffer1 = alloca [50 x i8], align 16
  %buffer2 = alloca [50 x i8], align 16
  %result_buffer = alloca [100 x i8], align 16
  %temp = alloca i8*, align 4
  %tt2 = alloca i8*, align 4
  %tmp = alloca i8*, align 4
  %tainted_str1 = alloca i8*, align 4
  %tmp4 = alloca i8*, align 4
  %tainted_str2 = alloca i8*, align 4
  %tmp9 = alloca i8*, align 4
  %tainted_result = alloca i8*, align 4
  %tmp14 = alloca i8*, align 4
  call void @sbx_init()
  %0 = call i64 @c_fetch_sandbox_heap_address()
  store i64 %0, i64* @sbxHeap, align 8
  %1 = call i64 @c_fetch_sandbox_heap_bound()
  %2 = call i64 @c_fetch_sandbox_heap_address()
  %3 = sub i64 %1, %2
  %4 = trunc i64 %3 to i32
  store i32 %4, i32* @sbxHeapRange, align 8
  store i32 0, i32* %retval, align 4
  %5 = bitcast [50 x i8]* %buffer1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %5, i8* align 16 getelementptr inbounds ([50 x i8], [50 x i8]* @__const.main.buffer1, i32 0, i32 0), i64 50, i1 false)
  %6 = bitcast [50 x i8]* %buffer2 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %6, i8 0, i64 50, i1 false)
  %7 = bitcast i8* %6 to [50 x i8]*
  %8 = getelementptr inbounds [50 x i8], [50 x i8]* %7, i32 0, i32 0
  store i8 87, i8* %8, align 16
  %9 = getelementptr inbounds [50 x i8], [50 x i8]* %7, i32 0, i32 1
  store i8 111, i8* %9, align 1
  %10 = getelementptr inbounds [50 x i8], [50 x i8]* %7, i32 0, i32 2
  store i8 114, i8* %10, align 2
  %11 = getelementptr inbounds [50 x i8], [50 x i8]* %7, i32 0, i32 3
  store i8 108, i8* %11, align 1
  %12 = getelementptr inbounds [50 x i8], [50 x i8]* %7, i32 0, i32 4
  store i8 100, i8* %12, align 4
  %13 = getelementptr inbounds [50 x i8], [50 x i8]* %7, i32 0, i32 5
  store i8 33, i8* %13, align 1
  %14 = bitcast i8** %temp to i64*
  store i64 0, i64* %14, align 4
  %15 = load i8*, i8** %temp, align 4
  %16 = load i32, i32* @sbxHeapRange, align 8
  %17 = load i64, i64* @sbxHeap, align 8
  %18 = ptrtoint i8* %15 to i32
  %19 = zext i32 %18 to i64
  %20 = add i64 %17, %19
  %21 = icmp ult i32 %18, %16
  br i1 %21, label %_Dynamic_check.succeeded, label %_Dynamic_check.failed

_Dynamic_check.succeeded:                         ; preds = %entry
  %22 = inttoptr i64 %20 to i8*
  %call = call i8* @C2T(i8* %22, i64 50)
  %23 = ptrtoint i8* %call to i32
  %24 = inttoptr i32 %23 to i8*
  store i8* %24, i8** %tmp, align 4
  %25 = load i8*, i8** %tmp, align 4
  %26 = ptrtoint i8* %25 to i32
  %27 = zext i32 %26 to i64
  %28 = bitcast i8** %tt2 to i64*
  store i64 %27, i64* %28, align 4
  %arraydecay = getelementptr inbounds [50 x i8], [50 x i8]* %buffer1, i64 0, i64 0
  %29 = load i32, i32* @sbxHeapRange, align 8
  %30 = load i64, i64* @sbxHeap, align 8
  %31 = ptrtoint i8* %arraydecay to i32
  %32 = zext i32 %31 to i64
  %33 = add i64 %30, %32
  %34 = icmp ult i32 %31, %29
  br i1 %34, label %_Dynamic_check.succeeded2, label %_Dynamic_check.failed1

_Dynamic_check.succeeded2:                        ; preds = %_Dynamic_check.succeeded
  %35 = inttoptr i64 %33 to i8*
  %call3 = call i8* @C2T(i8* %35, i64 50)
  %36 = ptrtoint i8* %call3 to i32
  %37 = inttoptr i32 %36 to i8*
  store i8* %37, i8** %tmp4, align 4
  %38 = load i8*, i8** %tmp4, align 4
  %39 = ptrtoint i8* %38 to i32
  %40 = zext i32 %39 to i64
  %41 = bitcast i8** %tainted_str1 to i64*
  store i64 %40, i64* %41, align 4
  %arraydecay5 = getelementptr inbounds [50 x i8], [50 x i8]* %buffer2, i64 0, i64 0
  %42 = load i32, i32* @sbxHeapRange, align 8
  %43 = load i64, i64* @sbxHeap, align 8
  %44 = ptrtoint i8* %arraydecay5 to i32
  %45 = zext i32 %44 to i64
  %46 = add i64 %43, %45
  %47 = icmp ult i32 %44, %42
  br i1 %47, label %_Dynamic_check.succeeded7, label %_Dynamic_check.failed6

_Dynamic_check.succeeded7:                        ; preds = %_Dynamic_check.succeeded2
  %48 = inttoptr i64 %46 to i8*
  %call8 = call i8* @C2T(i8* %48, i64 50)
  %49 = ptrtoint i8* %call8 to i32
  %50 = inttoptr i32 %49 to i8*
  store i8* %50, i8** %tmp9, align 4
  %51 = load i8*, i8** %tmp9, align 4
  %52 = ptrtoint i8* %51 to i32
  %53 = zext i32 %52 to i64
  %54 = bitcast i8** %tainted_str2 to i64*
  store i64 %53, i64* %54, align 4
  %arraydecay10 = getelementptr inbounds [100 x i8], [100 x i8]* %result_buffer, i64 0, i64 0
  %55 = load i32, i32* @sbxHeapRange, align 8
  %56 = load i64, i64* @sbxHeap, align 8
  %57 = ptrtoint i8* %arraydecay10 to i32
  %58 = zext i32 %57 to i64
  %59 = add i64 %56, %58
  %60 = icmp ult i32 %57, %55
  br i1 %60, label %_Dynamic_check.succeeded12, label %_Dynamic_check.failed11

_Dynamic_check.succeeded12:                       ; preds = %_Dynamic_check.succeeded7
  %61 = inttoptr i64 %59 to i8*
  %call13 = call i8* @C2T(i8* %61, i64 100)
  %62 = ptrtoint i8* %call13 to i32
  %63 = inttoptr i32 %62 to i8*
  store i8* %63, i8** %tmp14, align 4
  %64 = load i8*, i8** %tmp14, align 4
  %65 = ptrtoint i8* %64 to i32
  %66 = zext i32 %65 to i64
  %67 = bitcast i8** %tainted_result to i64*
  store i64 %66, i64* %67, align 4
  %68 = load i8*, i8** %tainted_str1, align 4
  %69 = load i8*, i8** %tainted_str2, align 4
  %70 = load i8*, i8** %tainted_result, align 4
  call void @perform_tainted_operation_with_callback(void (i8*, i8*, i8*)* @concatenate_tainted_strings, i8* %68, i8* %69, i8* %70, void (i8*)* @print_result_callback)
  %71 = load i8*, i8** %tainted_str1, align 4
  %72 = load i8*, i8** %tainted_str2, align 4
  %73 = load i8*, i8** %tainted_result, align 4
  call void @perform_tainted_operation_with_callback(void (i8*, i8*, i8*)* @copy_tainted_string, i8* %71, i8* %72, i8* %73, void (i8*)* @print_result_callback)
  ret i32 0

_Dynamic_check.failed:                            ; preds = %entry
  call void @llvm.trap() #7
  unreachable

_Dynamic_check.failed1:                           ; preds = %_Dynamic_check.succeeded
  call void @llvm.trap() #7
  unreachable

_Dynamic_check.failed6:                           ; preds = %_Dynamic_check.succeeded2
  call void @llvm.trap() #7
  unreachable

_Dynamic_check.failed11:                          ; preds = %_Dynamic_check.succeeded7
  call void @llvm.trap() #7
  unreachable
}

declare void @sbx_init()

declare i64 @c_fetch_sandbox_heap_address()

declare i64 @c_fetch_sandbox_heap_bound()

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #4

; Function Attrs: argmemonly nofree nosync nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #5

; Function Attrs: noinline nounwind optnone uwtable
define internal i8* @C2T(i8* %Ip, i64 %len) #0 {
entry:
  %Ip.addr = alloca i8*, align 8
  %len.addr = alloca i64, align 8
  %Iplen = alloca i32, align 4
  %RetPtr = alloca i8*, align 4
  %tmp = alloca i8*, align 4
  store i8* %Ip, i8** %Ip.addr, align 8
  store i64 %len, i64* %len.addr, align 8
  %0 = load i64, i64* %len.addr, align 8
  %conv = trunc i64 %0 to i32
  store i32 %conv, i32* %Iplen, align 4
  %1 = load i32, i32* %Iplen, align 4
  %conv1 = sext i32 %1 to i64
  %mul = mul i64 %conv1, 1
  %call = call i8* @string_tainted_malloc(i64 %mul)
  %2 = ptrtoint i8* %call to i32
  %3 = inttoptr i32 %2 to i8*
  store i8* %3, i8** %tmp, align 4
  %4 = load i8*, i8** %tmp, align 4
  %5 = ptrtoint i8* %4 to i32
  %6 = zext i32 %5 to i64
  %7 = bitcast i8** %RetPtr to i64*
  store i64 %6, i64* %7, align 4
  %8 = load i8*, i8** %RetPtr, align 4
  %9 = load i8*, i8** %Ip.addr, align 8
  %10 = load i32, i32* @sbxHeapRange, align 8
  %11 = load i64, i64* @sbxHeap, align 8
  %12 = ptrtoint i8* %8 to i32
  %13 = zext i32 %12 to i64
  %14 = add i64 %11, %13
  %15 = icmp ult i32 %12, %10
  br i1 %15, label %_Dynamic_check.succeeded, label %_Dynamic_check.failed

_Dynamic_check.succeeded:                         ; preds = %entry
  %16 = inttoptr i64 %14 to i8*
  %call2 = call i8* @strcpy(i8* %16, i8* %9) #6
  %17 = load i8*, i8** %RetPtr, align 4
  ret i8* %17

_Dynamic_check.failed:                            ; preds = %entry
  call void @llvm.trap() #7
  unreachable
}

; Function Attrs: noinline nounwind optnone uwtable
define internal i8* @string_tainted_malloc(i64 %sz) #0 {
entry:
  %retval = alloca i8*, align 8
  %sz.addr = alloca i64, align 8
  %p = alloca i8*, align 4
  %tmp = alloca i8*, align 4
  %tmp3 = alloca i8*, align 4
  %tmp5 = alloca i8*, align 4
  %tmp6 = alloca i8*, align 4
  %tmp7 = alloca i8*, align 4
  store i64 %sz, i64* %sz.addr, align 8
  %0 = load i64, i64* %sz.addr, align 8
  %cmp = icmp uge i64 %0, -1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i8* null, i8** %retval, align 8
  br label %return

if.end:                                           ; preds = %entry
  %1 = load i64, i64* %sz.addr, align 8
  %add = add i64 %1, 1
  %call = call i8* @t_malloc(i64 %add)
  %2 = ptrtoint i8* %call to i32
  %3 = inttoptr i32 %2 to i8*
  store i8* %3, i8** %tmp, align 4
  %4 = load i8*, i8** %tmp, align 4
  %5 = call i64 @c_fetch_sandbox_heap_bound()
  %6 = call i64 @c_fetch_sandbox_heap_address()
  %7 = sub i64 %5, %6
  %8 = trunc i64 %7 to i32
  store i32 %8, i32* @sbxHeapRange, align 8
  %9 = ptrtoint i8* %4 to i32
  %10 = zext i32 %9 to i64
  %11 = bitcast i8** %p to i64*
  store i64 %10, i64* %11, align 4
  %12 = load i8*, i8** %p, align 4
  %13 = ptrtoint i8* %12 to i32
  %14 = zext i32 %13 to i64
  %15 = inttoptr i64 %14 to i8*
  %cmp1 = icmp ne i8* %15, null
  br i1 %cmp1, label %if.then2, label %if.end10

if.then2:                                         ; preds = %if.end
  %16 = load i8*, i8** %p, align 4
  %17 = load i64, i64* %sz.addr, align 8
  %18 = load i32, i32* @sbxHeapRange, align 8
  %19 = load i64, i64* @sbxHeap, align 8
  %20 = ptrtoint i8* %16 to i32
  %21 = zext i32 %20 to i64
  %22 = add i64 %19, %21
  %23 = icmp ult i32 %20, %18
  br i1 %23, label %_Dynamic_check.succeeded, label %_Dynamic_check.failed

_Dynamic_check.succeeded:                         ; preds = %if.then2
  %24 = inttoptr i64 %22 to i8*
  %arrayidx = getelementptr inbounds i8, i8* %24, i64 %17
  %25 = ptrtoint i8* %arrayidx to i32
  %26 = inttoptr i32 %25 to i8*
  store i8* %26, i8** %tmp3, align 4
  %27 = load i8*, i8** %tmp3, align 4
  %28 = load i8*, i8** %p, align 4
  %29 = load i8*, i8** %p, align 4
  %30 = load i64, i64* %sz.addr, align 8
  %add4 = add i64 %30, 1
  %add.ptr = getelementptr inbounds i8, i8* %29, i64 %add4
  store i8* %add.ptr, i8** %tmp5, align 4
  %31 = load i8*, i8** %tmp5, align 4
  %32 = ptrtoint i8* %31 to i32
  %33 = inttoptr i32 %32 to i8*
  store i8* %33, i8** %tmp6, align 4
  %34 = load i8*, i8** %tmp6, align 4
  %35 = ptrtoint i8* %28 to i32
  %36 = inttoptr i32 %35 to i8*
  store i8* %36, i8** %tmp7, align 4
  %37 = load i8*, i8** %tmp7, align 4
  %_Dynamic_check.lower = icmp ule i8* %37, %27
  %_Dynamic_check.upper = icmp ult i8* %27, %34
  %_Dynamic_check.range = and i1 %_Dynamic_check.lower, %_Dynamic_check.upper
  br i1 %_Dynamic_check.range, label %_Dynamic_check.succeeded9, label %_Dynamic_check.failed8

_Dynamic_check.succeeded9:                        ; preds = %_Dynamic_check.succeeded
  store i8 0, i8* %arrayidx, align 4
  br label %if.end10

_Dynamic_check.failed:                            ; preds = %if.then2
  call void @llvm.trap() #7
  unreachable

_Dynamic_check.failed8:                           ; preds = %_Dynamic_check.succeeded
  call void @llvm.trap() #7
  unreachable

if.end10:                                         ; preds = %_Dynamic_check.succeeded9, %if.end
  %38 = load i8*, i8** %p, align 4
  store i8* %38, i8** %retval, align 8
  br label %return

return:                                           ; preds = %if.end10, %if.then
  %39 = load i8*, i8** %retval, align 8
  ret i8* %39
}

declare dso_local i8* @t_malloc(i64) #3

attributes #0 = { noinline nounwind optnone uwtable "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { cold noreturn nounwind }
attributes #3 = { "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nofree nosync nounwind willreturn }
attributes #5 = { argmemonly nofree nosync nounwind willreturn writeonly }
attributes #6 = { nounwind }
attributes #7 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 12.0.0 (git@github.com:TypeFlexer/TypeFlexer-Clang.git 0ada8bb1405cd4fe0d611f381f1b631a829a48ff)"}
