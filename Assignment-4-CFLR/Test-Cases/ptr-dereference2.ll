; ModuleID = 'Assignment-4-CFLR/Test-Cases/ptr-dereference2.c'
source_filename = "Assignment-4-CFLR/Test-Cases/ptr-dereference2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca ptr, align 8
  %b = alloca ptr, align 8
  %x = alloca ptr, align 8
  %c = alloca i32, align 4
  %y = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  store i32 10, ptr %c, align 4
  store ptr %b, ptr %a, align 8
  store ptr %c, ptr %b, align 8
  %0 = load ptr, ptr %a, align 8
  %1 = load ptr, ptr %0, align 8
  store ptr %1, ptr %x, align 8
  %2 = load ptr, ptr %x, align 8
  %3 = load i32, ptr %2, align 4
  store i32 %3, ptr %y, align 4
  %4 = load ptr, ptr %x, align 8
  call void @MUSTALIAS(ptr noundef %4, ptr noundef %c)
  %5 = load ptr, ptr %x, align 8
  %6 = load ptr, ptr %b, align 8
  call void @MUSTALIAS(ptr noundef %5, ptr noundef %6)
  ret i32 0
}

declare void @MUSTALIAS(ptr noundef, ptr noundef) #1

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 16.0.4 (https://github.com/bjjwwang/LLVM-compile 2259018fffaba36b5e50dce52fb5aa97b417ce4f)"}
