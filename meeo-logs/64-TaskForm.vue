<template>
  <div class="create-task-container">
    <!-- 外层卡片 -->
    <div class="page-card">
      <el-row :gutter="24">
        <!-- 左侧：GitHub 提交详情 卡片 -->
        <el-col :span="12">
          <el-card shadow="never" class="section-card">
            <template #header>
              <div class="section-header">
                <el-icon>
                  <MessageBox />
                </el-icon>
                <span>GitHub 提交详情</span>
                <el-button type="text" class="refresh-btn" @click="handleSync" :disabled="isSyncing">
                  <el-icon :class="{ 'is-loading': isSyncing }">
                    <RefreshRight />
                  </el-icon>
                </el-button>
              </div>
            </template>

            <!-- 提交信息规范示例 -->
            <el-alert v-if="props.taskId" class="instruction-alert" type="info" :closable="false" show-icon
              :title="`Commit Message 中请包含任务 ID 前缀：[T${props.taskId}]`" />
            <el-alert v-else class="instruction-alert" type="warning" :closable="false" show-icon
              title="请先保存任务以生成 taskId，后续 Commit Message 中需包含前缀 [T<taskId>]" />

            <div class="timeline-container">
              <template v-if="stageDetails.length">
                <el-timeline v-if="stageDetails.length">
                  <el-timeline-item v-for="(stage, i) in stageDetails" :key="i" :timestamp="stage.timestamp"
                    placement="top">
                    <el-card class="timeline-card">
                      <!-- 头部：头像 + 昵称 + 时间 -->
                      <div class="timeline-header">
                        <el-avatar :src="stage.avatar" size="small" />
                        <span class="timeline-nick">{{ stage.nickname }}</span>
                        <span class="timeline-author">by {{ stage.author }}</span>
                      </div>
                      <!-- 主体：消息 + 统计 -->
                      <div class="timeline-body">
                        <p class="timeline-message">
                          <el-icon v-if="stage.isMerge">
                            <Branch />
                          </el-icon>
                          {{ stage.message }}
                        </p>
                        <div class="timeline-stats">
                          <el-icon>
                            <Plus />
                          </el-icon><span class="stat-num">{{ stage.additions }}</span>
                          <el-icon>
                            <Minus />
                          </el-icon><span class="stat-num">{{ stage.deletions }}</span>
                        </div>
                      </div>
                    </el-card>
                  </el-timeline-item>
                </el-timeline>
              </template>
              <div v-else class="timeline-placeholder">暂无提交记录</div>
            </div>
          </el-card>

        </el-col>

        <!-- 右侧：表单与详情 -->
        <el-col :span="12">
          <el-form :model="form" :rules="rules" ref="formRef" label-width="0" class="task-form">
            <!-- 任务编辑卡片 -->
            <el-card shadow="hover" class="section-card">
              <template #header>
                <div class="section-header">
                  <el-icon>
                    <Edit />
                  </el-icon>
                  <span>任务信息</span>
                </div>
              </template>

              <el-row :gutter="16" class="form-grid">
                <el-col :span="12">
                  <el-form-item prop="title">
                    <el-input v-model="form.title" placeholder="任务标题" class="input-borderless" />
                  </el-form-item>
                </el-col>
                <el-col :span="12">
                  <el-form-item prop="assigneeId">
                    <el-select v-model="form.assigneeId" placeholder="负责人" class="input-borderless">
                      <el-option v-for="u in projectUsers" :key="u.userId" :label="u.nickname" :value="u.userId">
                        <div class="user-option">
                          <el-avatar :src="u.avatar" size="small" />
                          {{ u.nickname }}
                        </div>
                      </el-option>
                    </el-select>
                  </el-form-item>
                </el-col>

                <el-col :span="12">
                  <el-form-item prop="currentStage">
                    <el-select v-model="form.currentStage" placeholder="任务阶段" class="input-borderless">
                      <el-option v-for="s in ['需求分析','系统设计','项目开发','功能测试']" :key="s" :label="s" :value="s" />
                    </el-select>
                  </el-form-item>
                </el-col>
                <el-col :span="12">
                  <el-form-item prop="priority">
                    <el-select v-model="form.priority" placeholder="优先级" class="input-borderless">
                      <el-option v-for="p in ['low','medium','high','urgent']" :key="p" :label="{
                          low: '低',
                          medium: '中',
                          high: '高',
                          urgent: '紧急'
                        }[p]" :value="p" />
                    </el-select>
                  </el-form-item>
                </el-col>

                <el-col :span="24">
                  <el-form-item prop="description">
                    <el-input type="textarea" v-model="form.description" placeholder="任务描述"
                      :autosize="{ minRows: 3, maxRows: 5 }" class="input-borderless textarea-borderless" />
                  </el-form-item>
                </el-col>

                <el-col :span="12">
                  <el-form-item prop="estimatedHours">
                    <el-input v-model="form.estimatedHours" placeholder="预估工时(h)" type="number"
                      class="input-borderless" />
                  </el-form-item>
                </el-col>
                <el-col :span="12">
                  <el-form-item prop="plannedTimeRange">
                    <el-date-picker v-model="form.plannedTimeRange" type="datetimerange" start-placeholder="开始"
                      end-placeholder="结束" :default-time="[defaultTime, defaultTime]" class="input-borderless"
                      style="width:100%;" format="YYYY-MM-DD HH:mm:ss" value-format="YYYY-MM-DD HH:mm:ss" />
                  </el-form-item>
                </el-col>
              </el-row>
            </el-card>

            <!-- 只读详情卡片 -->
            <el-card shadow="hover" class="section-card">
              <template #header>
                <div class="section-header">
                  <el-icon>
                    <InfoFilled />
                  </el-icon>
                  <span>任务详情</span>
                </div>
              </template>
              <el-descriptions :column="2" border class="read-only-desc">
                <el-descriptions-item label="创建者">
                  {{ form.creatorNickname }}
                </el-descriptions-item>
                <el-descriptions-item label="状态">
                  {{ form.status }}
                </el-descriptions-item>
                <el-descriptions-item label="已用工时">
                  {{ form.spentHours || '0'}}h
                </el-descriptions-item>
                <el-descriptions-item label="创建时间">
                  {{ form.createTime || '-' }}
                </el-descriptions-item>
                <el-descriptions-item label="实际开始">
                  {{ form.actualStartTime || '-' }}
                </el-descriptions-item>
                <el-descriptions-item label="实际结束">
                  {{ form.actualEndTime || '-' }}
                </el-descriptions-item>
                <el-descriptions-item label="更新时间">
                  {{ form.updateTime || '-' }}
                </el-descriptions-item>
              </el-descriptions>

              <div class="task-details-actions">
                <el-button v-if="props.taskId" type="danger" @click="deleteCurrentTask">删除</el-button>
                <el-button type="primary" @click="submitForm">提交</el-button>
              </div>
            </el-card>
          </el-form>
        </el-col>
      </el-row>
    </div>
  </div>
</template>

<script setup lang="ts">
  import { ref, onMounted, watch, computed } from 'vue'
  import { useRoute } from 'vue-router'
  import { ElMessage, ElMessageBox } from 'element-plus'
  import { useCounterStore } from '@/stores/counter'
  import { createTask, getTaskById, updateTask, deleteTask, } from '@/api/task'
  import { getProject, getProjectUsers } from '@/api/project'
  import dayjs from 'dayjs'
  import { syncGitHubAll, getGitHubCommitsByTask } from '@/api/github/githubSync.ts'

  const props = defineProps < {
    taskId?: number | null,
    projectId?: number | null
  } > ()
  const emit = defineEmits(['submit', 'cancel'])

  const route = useRoute()
  const counterStore = useCounterStore()
  const currentUserId = counterStore.userInfo.id
  const formRef = ref(null)
  const isSyncing = ref(false)

  const ParamsId = Number(route.params.id)
  const projectId = computed < number | null > (() => {
    return !isNaN(ParamsId) && ParamsId > 0
      ? ParamsId
      : (props.projectId ?? null)
  })
  const projectStart = ref < number > (0)
  const projectEnd = ref < number > (0)

  const form = ref({
    projectId,
    title: '',
    description: '',
    status: 'todo',
    priority: 'low',
    currentStage: '需求分析',
    assigneeId: '',
    creatorId: currentUserId,
    estimatedHours: '',
    plannedTimeRange: [new Date(), new Date()],
    creatorNickname: '',
    spentHours: '',
    actualStartTime: null,
    actualEndTime: null,
    createTime: null,
    updateTime: null,
  })

  const stageDetails = ref < any[] > ([])
  const projectUsers = ref < any[] > ([])
  const defaultTime = new Date(2000, 1, 1, 12, 0, 0)

  const rules = {
    title: [
      { required: true, message: '任务标题不能为空', trigger: 'blur' }
    ],
    assigneeId: [
      { required: true, message: '请选择负责人', trigger: 'blur' }
    ],
    estimatedHours: [
      { required: true, message: '请输入预估工时', trigger: 'blur' },
    ],
    plannedTimeRange: [
      { required: true, message: '请选择计划时间范围', trigger: 'change' },
      // 自定义范围校验
      {
        validator(_rule, value: [string, string], callback) {
          if (!value || value.length !== 2) {
            return callback(new Error('请选择计划时间范围'))
          }
          const [start, end] = value
          const s = dayjs(start).valueOf()
          const e = dayjs(end).valueOf()
          if (s < projectStart.value || e > projectEnd.value) {
            const fmt = (ts: number) => dayjs(ts).format('YYYY-MM-DD HH:mm')
            return callback(new Error(
              `计划时间必须在项目范围内：${fmt(projectStart.value)} ～ ${fmt(projectEnd.value)}`
            ))
          }
          callback()
        },
        trigger: 'change'
      }
    ]
  }

  const loadCommits = async (taskId: number) => {
    try {
      const res = await getGitHubCommitsByTask(projectId.value, taskId)
      if (res.code === 200) {
        stageDetails.value = res.data.map((c: any) => ({
          avatar: c.avatar,
          nickname: c.nickname,
          author: c.author,
          timestamp: dayjs(c.commitDate).format('YYYY-MM-DD HH:mm:ss'),
          message: c.message,
          additions: c.additions,
          deletions: c.deletions,
          isMerge: c.isMerge,
        }))
      } else {
        ElMessage.error('获取提交记录失败')
      }
    } catch (err) {
      console.error(err)
      ElMessage.error('加载提交记录异常')
    }
  }

  // 点击刷新
  const handleSync = async () => {
    isSyncing.value = true
    try {
      const res = await syncGitHubAll(projectId.value)
      if (res.code === 200) {
        ElMessage.success('仓库已同步')
        await loadCommits(props.taskId)
      } else {
        throw new Error(res.message || '同步失败')
      }
    } catch (e: any) {
      ElMessage.error(e.message || '同步失败')
    } finally {
      isSyncing.value = false
    }
  }


  watch(
    () => props.taskId,
    (newId) => {
      if (newId) {
        loadCommits(newId)
      } else {
        stageDetails.value = []
      }
    },
    { immediate: true }
  )

  onMounted(async () => {
    try {
      const resProj = await getProject(projectId.value)
      console.log(resProj)
      if (resProj.code === 200) {
        projectStart.value = dayjs(resProj.data.startDate).valueOf()
        projectEnd.value = dayjs(resProj.data.endDate).valueOf()
      }
    } catch {
      ElMessage.error('无法获取项目起止时间')
    }

    try {
      const userRes = await getProjectUsers(projectId.value)
      projectUsers.value =
        userRes.code === 200 ? userRes.data : []
    } catch {
      ElMessage.error('获取项目成员失败')
    }

    if (props.taskId) {
      const detailRes = await getTaskById(props.taskId)
      if (detailRes.code === 200) {
        form.value = detailRes.data
        form.value.createTime = dayjs(form.value.createTime).format('YYYY-MM-DD HH:mm:ss')
        form.value.updateTime = dayjs(form.value.updateTime).format('YYYY-MM-DD HH:mm:ss')
        if (form.value.actualStartTime) form.value.actualStartTime = dayjs(form.value.actualStartTime).format('YYYY-MM-DD HH:mm:ss')
        if (form.value.actualEndTime) form.value.actualEndTime = dayjs(form.value.actualEndTime).format('YYYY-MM-DD HH:mm:ss')
        const { plannedStartTime, plannedEndTime } = form.value
        if (plannedStartTime && plannedEndTime) {
          form.value.plannedTimeRange = [
            dayjs(plannedStartTime).format('YYYY-MM-DD HH:mm:ss'),
            dayjs(plannedEndTime).format('YYYY-MM-DD HH:mm:ss'),
          ]
        }
      }
    }
  })

  const submitForm = () => {
    formRef.value?.validate(async (valid: boolean) => {
      if (!valid) return
      const [start, end] = form.value.plannedTimeRange || []
      form.value.plannedStartTime = start
      form.value.plannedEndTime = end

      try {
        if (props.taskId) {
          const updateRes = await updateTask(props.taskId, form.value)
          updateRes.code === 200
            ? (ElMessage.success('任务更新成功'), emit('submit'))
            : ElMessage.error('任务更新失败')
        } else {
          const createRes = await createTask(form.value)
          createRes.code === 200
            ? (ElMessage.success('任务创建成功'), emit('submit'))
            : ElMessage.error('任务创建失败')
        }
      } catch {
        ElMessage.error('操作失败')
      }
    })
  }

  const deleteCurrentTask = () => {
    ElMessageBox.confirm('确认删除该任务吗？', '删除任务', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning',
    })
      .then(async () => {
        const res = await deleteTask(props.taskId!)
        res.code === 200
          ? (ElMessage.warning('任务删除成功'), emit('submit'))
          : ElMessage.error('任务删除失败')
      })
      .catch(() => { })
  }
</script>

<style scoped>
  .create-task-container {
    padding: 15px;
  }

  .section-card {
    margin-bottom: 20px;
    border-radius: 8px;
    --el-card-box-shadow: 0 2px 8px rgba(0, 0, 0, 0.04);
  }

  .section-header {
    display: flex;
    align-items: center;
    font-size: 16px;
    font-weight: 600;
    color: #303133;
  }

  .section-header span {
    margin-left: 6px;
  }

  .refresh-btn {
    margin-left: auto;
  }

  .form-grid {
    margin-top: 12px;
  }

  .user-option {
    display: flex;
    align-items: center;
  }

  .el-avatar {
    flex-shrink: 0;
    margin-right: 4px;
    border-radius: 0;
    background-color: transparent;
    box-shadow: none;
  }

  .task-details-actions {
    text-align: right;
    margin-top: 16px;
  }

  .task-details-actions .el-button {
    margin-left: 8px;
  }

  /* GitHub提交 TimeLine */
  .timeline-placeholder {
    height: 180px;
    display: flex;
    align-items: center;
    justify-content: center;
    color: #909399;
    font-size: 14px;
  }

  .timeline-container {
    padding: 15px;
    height: 536px;
  }

  .timeline-card {
    background: #fafafa;
  }

  .timeline-header {
    display: flex;
    align-items: center;
    margin-bottom: 8px;
  }

  .timeline-nick {
    margin: 0 8px;
    font-weight: 600;
    color: #333;
  }

  .timeline-author {
    margin-left: auto;
    font-size: 12px;
    color: #999;
  }

  .timeline-body {
    padding-left: 32px;
  }

  .timeline-message {
    margin: 0 0 6px;
    color: #606266;
  }

  .timeline-stats {
    display: flex;
    align-items: center;
    font-size: 12px;
    color: #909399;
  }

  .timeline-stats .stat-num {
    margin: 0 12px 0 4px;
  }
</style>