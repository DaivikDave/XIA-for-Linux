#include <linux/init.h>
#include <linux/module.h>
#include <net/udp.h>
#include <net/udp_tunnel.h>
#include <net/xia_dag.h>
#include <net/xia_list_fib.h>
#include <net/xia_vxidty.h>
#include <uapi/linux/udp.h>

/* U6ID Principal */
#define XIDTYPE_U6ID (__cpu_to_be32(0x1b))

static int __net_init u6id_net_init(struct net *net)
{

}

static void __net_exit u6id_net_exit(struct net *net)
{

}

static struct pernet_operations u6id_net_ops __read_mostly = {
    .init = u6id_net_init,
    .exit = u6id_net_exit,
};

static int u6id_deliver(void) {

}

static struct xip_route_proc u6id_rt_proc __read_mostly = {
    .xrp_ppal_type = XIDTYPE_U6ID,
    .deliver = u6id_deliver,
};

static __init xia_u6id_init(void)
{
    int rc;

    rc = vxt_register_xidty(XIDTYPE_U6ID);
    if(rc < 0) {
        pr_err("Can't obtain a virtual XID type for U6ID\n");
        goto out;
    }
    my_vxt = rc;
    
    rc = xia_register_pernet_subsys(&u6id_net_ops);
    if (rc)
        goto vxt;
    
    rc = xip_add_router(&u6id_rt_proc);
    if(rc)
        goto net;

    rc = ppal_add_map("u6id",XIDTYPE_U6ID);
    if(rc)
        goto route;

    printk(KERN_ALERT "XIA Principal U6ID loaded\n");
    goto out;

route:
    xip_del_router(&u6id_rt_proc);
net:
    xia_unregister_pernet_subsys(&u6id_net_ops);
vxt:
    BUG_ON(vxt_unregister_xidty(XIDTYPE_U6ID));
out:
    return rc;
}

static void __exit xia_u6id_exit(void)
{
    ppal_del_map(XIDTYPE_U6ID);
    xip_del_router(&u6id_rt_proc);
    xia_unregister_pernet_subsys(&u6id_net_ops);
    BUG_ON(vxt_unregister_xidty(XIDTYPE_U6ID));
    
    rcu_barrier();
    flush_scheduled_work();

    printk(KERN_ALERT "XIA Principal U6ID Unloaded\n");
}

module_init(xia_u6id_init);
module_exit(xia_u6id_exit);

MODULE_LICENCE("GPL");
MODULE_DESCRIPTION("XIA UDP/IPv6 Principal");
