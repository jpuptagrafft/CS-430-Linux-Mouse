/*KEYBOARD MOUSE
KNOWN ISSUES
 - After first release, Mouse Click no longer works.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/proc_fs.h>
#include <linux/keyboard.h>
#include <asm/io.h>


#define UP 0x3
#define DOWN 0x0
#define LEFT 0x1
#define RIGHT 0x2



struct input_dev *jerry; //Our mouse... It's named Jerry because then I can give it agency, then yell at it when things go wrong... it makes me feel better...
struct notifier_block nb; //our keyboard

int state = 0; //Mouse state: 0 is up, 1 is down... something weird is going on here...

int n = 25;

int move_mouse(struct notifier_block *nb, unsigned long action, void* data){
	struct keyboard_notifier_param *kp = (struct keyboard_notifier_param*)data;
	int valuePartOne = (kp->value) & 0xff;
	int valuePartTwo = (kp->value >> 8) & 0x0f;
	if(kp->down == 0){
		switch(valuePartTwo){ 
			case 0x0: //Any normal Key
				if(valuePartOne == 0x77){// Using the Pause/Break button
					if(state == 0)
						state = 1;
					else
						state = 0;
					input_report_key(jerry, BTN_LEFT, state); 
				}
				break;
				
			case 0x6: //Arrow keys
				if(valuePartOne == UP){
					input_report_rel(jerry, REL_Y, -n); //Default Y Value is Inverted. Corrected for this Product.
				} else if(valuePartOne == DOWN){
					input_report_rel(jerry, REL_Y, n);  //Default Y Value is Inverted. Corrected for this Product.
				} else if(valuePartOne == RIGHT){
					input_report_rel(jerry, REL_X, n); 
				} else if(valuePartOne == LEFT){
					input_report_rel(jerry, REL_X, -n); 
				}
				break;
		}
		input_sync(jerry);
	}
	return 0;	
}

static int __init mm_init(void)
{
	nb.notifier_call = move_mouse;
	register_keyboard_notifier(&nb);
	jerry = input_allocate_device();
	jerry->name = "jerry";
	set_bit(EV_REL, jerry->evbit);
	set_bit(REL_X, jerry->relbit);
	set_bit(REL_Y, jerry->relbit);
	
	set_bit(EV_KEY, jerry->evbit);
	set_bit(BTN_LEFT, jerry->keybit);
	
	input_register_device(jerry);

	return 0;
}

static void __exit mm_remove(void)
{
	unregister_keyboard_notifier(&nb);
	input_unregister_device(jerry);
}

MODULE_LICENSE("GPL"); 
module_init(mm_init);
module_exit(mm_remove);
