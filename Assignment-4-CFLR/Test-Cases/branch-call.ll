; ModuleID = 'Assignment-4-CFLR/Test-Cases/branch-call.c'
source_filename = "Assignment-4-CFLR/Test-Cases/branch-call.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @foo(ptr noundef %m, ptr noundef %n) #0 {
entry:
  %m.addr = alloca ptr, align 8
  %n.addr = alloca ptr, align 8
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  store ptr %m, ptr %m.addr, align 8
  store ptr %n, ptr %n.addr, align 8
  %0 = load ptr, ptr %n.addr, align 8
  %1 = load i32, ptr %0, align 4
  store i32 %1, ptr %x, align 4
  %2 = load ptr, ptr %m.addr, align 8
  %3 = load i32, ptr %2, align 4
  store i32 %3, ptr %y, align 4
  %4 = load i32, ptr %x, align 4
  %5 = load ptr, ptr %m.addr, align 8
  store i32 %4, ptr %5, align 4
  %6 = load i32, ptr %y, align 4
  %7 = load ptr, ptr %n.addr, align 8
  store i32 %6, ptr %7, align 4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %p = alloca ptr, align 8
  %q = alloca ptr, align 8
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  %0 = load i32, ptr %c, align 4
  %tobool = icmp ne i32 %0, 0
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  store ptr %a, ptr %p, align 8
  store ptr %b, ptr %q, align 8
  %1 = load ptr, ptr %p, align 8
  %2 = load ptr, ptr %q, align 8
  call void @foo(ptr noundef %1, ptr noundef %2)
  br label %if.end

if.else:                                          ; preds = %entry
  store ptr %b, ptr %p, align 8
  store ptr %c, ptr %q, align 8
  %3 = load ptr, ptr %p, align 8
  %4 = load ptr, ptr %q, align 8
  call void @foo(ptr noundef %3, ptr noundef %4)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 16.0.4 (https://github.com/bjjwwang/LLVM-compile 2259018fffaba36b5e50dce52fb5aa97b417ce4f)"}
